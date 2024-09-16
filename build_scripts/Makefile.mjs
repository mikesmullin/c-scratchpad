import { glob } from 'glob'
import cbFs from 'fs';
import fs from 'fs/promises';
import path from 'path';
import { spawn } from 'child_process';
import { fileURLToPath } from 'url';

const isWin = process.platform === "win32";
const isMac = process.platform === "darwin";
const isNix = process.platform === "linux";
const __dirname = path.dirname(fileURLToPath(import.meta.url));
const CONCURRENCY = 48; // threadripper ftw!
// NOTICE: assumes glslc is in path
const GLSLC_PATH = isWin ? path.join('glslc.exe') : path.join('glslc');
const BUILD_PATH = "build";
//const CPP_COMPILER_PATH = "clang++";
const C_COMPILER_PATH = 'clang';
const RX_EXT = /\.[\w\d]{1,3}$/i;
const RX_C = /\.c$/i;
const OUT_FILE = "compile_commands.json";
const abs = (...args) => path.join(...args);
const workspaceFolder = path.join(__dirname, '..');
const rel = (...args) =>
  path.relative(path.join(workspaceFolder, BUILD_PATH), path.join(...args));
const DEBUG_COMPILER_ARGS = [
  '-O0',
  '-gdwarf', // adds gdb support
  // TODO: do we need to pass `-debug` to the linker? `-Xlinker -debug`?
];
const C_COMPILER_ARGS = [];
C_COMPILER_ARGS.push('-m64');
const C_COMPILER_INCLUDES = [];

const LINKER_LIBS = [];
// NOTICE: we can lookup the compiler flags using command `sdl2-config --cflags --libs`
const LINKER_LIB_PATHS = [];
const COMPILER_TRANSLATION_UNITS = [
  rel(workspaceFolder, 'src', 'tests', 'unit', '*.c'),
  rel(workspaceFolder, 'src', 'lib', '*.c'),
];

const generate_clangd_compile_commands = async () => {
  console.log('scanning directory...');
  const unit_files = await glob('{src,tests}/**/*.c');

  console.debug('unit_files: ', unit_files);

  const compile_commands = [];

  for (const unit_file of unit_files) {
    compile_commands.push({
      directory: path.join(workspaceFolder, BUILD_PATH),
      arguments: [
        C_COMPILER_PATH,
        ...C_COMPILER_ARGS,
        ...C_COMPILER_INCLUDES,
        '-c',
        '-o', `${unit_file}.o`,
        rel(unit_file),
      ],
      file: rel(workspaceFolder, unit_file),
    });
  }

  console.log(`writing ${OUT_FILE}...`)
  await fs.writeFile(OUT_FILE, JSON.stringify(compile_commands, null, 2));

  console.log('done making.');
};

const child_spawn = async (cmd, args = [], opts = {}) => {
  const cwd = path.relative(process.cwd(), path.join(workspaceFolder, BUILD_PATH));
  // console.log(`cd ${cwd}`);
  console.log(`${opts.stdin ? `type ${opts.stdin} | ` : ''}${cmd} ${args.join(' ')}${opts.stdout ? ` > ${opts.stdout}` : ''}`);
  let stdin, stdout;
  const stdio = ['inherit', 'inherit', 'inherit'];
  if (opts.stdin) {
    stdio[0] = 'pipe';
    stdin = cbFs.createReadStream(path.join(workspaceFolder, 'assets', 'proto', 'addressbook.pb'));
  }
  if (opts.stdout) {
    stdio[1] = 'pipe';
    stdout = await cbFs.createWriteStream(path.join(workspaceFolder, 'assets', 'proto', 'addressbook.bin'));
  }
  const child = spawn(cmd, args, { cwd, stdio });
  if (opts.stdin) {
    stdin.pipe(child.stdin);
  }
  if (opts.stdout) {
    child.stdout.pipe(stdout);
  }
  const code = await new Promise((ok) => {
    child.on('close', async (code) => {
      if (code !== 0) {
        console.log(`process exited with code ${code}`);
      }
      ok(code);
    });
  });
  return code;
};

const promiseBatch = async function* (concurrency, list, fn) {
  for (let p = [], i = 0, l = list.length; i < l || p.length > 0;) {
    if (i < l) {
      let _p;
      _p = fn(list[i]).then(r => [_p.__id, r]);
      _p.__id = i++;
      if (p.push(_p) < concurrency) {
        continue;
      }
    }
    const [id, res] = await Promise.race(p);
    p = p.filter(x => x.__id !== id);
    yield res;
  }
};

const all = async () => {
  await clean();
  await compile('main');
  await run('main');
};

const clean = async () => {
  await fs.rm(path.join(workspaceFolder, BUILD_PATH), { recursive: true, force: true });
  await fs.mkdir(path.join(workspaceFolder, BUILD_PATH));
};

const compile = async (basename) => {
  console.log(`compiling ${basename}...`);
  const absBuild = (...args) => path.join(workspaceFolder, BUILD_PATH, ...args);

  // compile translation units in parallel (N-at-once)
  const main = `src/${basename}.c`;
  const unit_files = [main];
  const dsts = [`${main}.o`];
  for (const u of COMPILER_TRANSLATION_UNITS) {
    for (const file of await glob(path.relative(workspaceFolder, absBuild(u)).replace(/\\/g, '/'))) {
      unit_files.push(file);
      dsts.push(rel(workspaceFolder, BUILD_PATH, `${file}.o`));
    }
  }
  const compileTranslationUnit = async (unit) => {
    const dir = path.relative(process.cwd(), absBuild(path.dirname(unit)));
    await fs.mkdir(dir, { recursive: true });

    const src = rel(workspaceFolder, unit);
    const dst = rel(workspaceFolder, BUILD_PATH, `${unit}.o`);

    let dstExists = false;
    try {
      await fs.access(path.join(BUILD_PATH, dst), fs.constants.F_OK);
      dstExists = true;
    }
    catch (e) {
    }
    if (dstExists) {
      const srcStat = await fs.stat(path.join(BUILD_PATH, src));
      const dstStat = await fs.stat(path.join(BUILD_PATH, dst));
      if (srcStat.mtime < dstStat.mtime) {
        return;
      }
    }

    const is_c = RX_C.test(src);
    await child_spawn((is_c ? C_COMPILER_PATH : CPP_COMPILER_PATH), [
      ...DEBUG_COMPILER_ARGS,
      ...C_COMPILER_ARGS,
      ...C_COMPILER_INCLUDES,
      src,
      '-c',
      '-o', dst,
    ]);

    return dst;
  };
  const objs = [];
  for await (const obj of promiseBatch(CONCURRENCY, unit_files, compileTranslationUnit)) {
    if (obj) {
      objs.push(obj);
    }
  }

  // linker stage
  const executable = `${basename}${isWin ? '.exe' : ''} `;
  const exePath = path.join(workspaceFolder, BUILD_PATH, executable);

  let code = 0;
  if (objs.length > 0) {
    code = await child_spawn(C_COMPILER_PATH, [
      ...DEBUG_COMPILER_ARGS,
      ...C_COMPILER_ARGS,
      ...C_COMPILER_INCLUDES,
      ...LINKER_LIBS,
      ...LINKER_LIB_PATHS,
      ...dsts,
      '-o', executable
    ]);
  }
  console.log("done compiling.");
};

const run = async (basename) => {
  const executable = `${basename}${isWin ? '.exe' : ''}`;
  const exePath = path.join(workspaceFolder, BUILD_PATH, executable);
  try {
    await fs.stat(exePath);
  } catch (e) {
    console.log(".exe is missing. probably failed to compile.", e);
    return;
  }

  if (isNix || isMac) {
    // chmod +x
    await fs.chmod(exePath, 0o755);
  }
  await child_spawn(exePath);
}

const watch = async () => {
  console.log(`watching...`);
  const watcher = fs.watch(path.join(workspaceFolder, 'src', 'tests', 'unit', 'test001.c'));
  let timer;
  let wait = false;
  for await (const event of watcher) {
    clearTimeout(timer);
    if (!wait) {
      timer = setTimeout(async () => {
        wait = true;
        await all();
        wait = false;
      }, 250);
    }
  }
};

(async () => {
  const [, , ...cmds] = process.argv;
  loop:
  for (const cmd of cmds) {
    switch (cmd) {
      case 'all':
        all();
        break;
      case 'clean':
        await clean();
        break;
      case 'compile_commands':
        await generate_clangd_compile_commands();
        break;
      case 'main':
        await compile(cmd);
        break;
      case 'watch':
        await watch();
        break;
      case 'help':
      default:
        console.log(`
Mike's hand-rolled build system.

  USAGE:
  node build_scripts\\Makefile.mjs < SUBCOMMAND >

    SUBCOMMANDS:
  all
  Clean, rebuild, and launch the default app.
    clean
    Delete all build output.
    compile_commands
    Generate the.json file needed for clangd for vscode extension.
    main
    Compile and run the main app
    `);
        break loop;
    }
  }
})();