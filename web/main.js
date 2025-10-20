const DISPLAY_WIDTH = 160;
const DISPLAY_HEIGHT = 80;
const DISPLAY_BUFFER_SIZE = (DISPLAY_WIDTH * DISPLAY_HEIGHT) / 8;

const canvas = document.getElementById("lava-canvas");
const ctx = canvas.getContext("2d", { alpha: false });
const scaleInput = document.getElementById("scale-input");
const scaleValue = document.getElementById("scale-value");
const keyboardPanel = document.querySelector(".keyboard");
const fsButton = document.getElementById("fs-button");
const screenshotButton = document.getElementById("screenshot-button");

const imageData = ctx.createImageData(DISPLAY_WIDTH, DISPLAY_HEIGHT);
const pixelBuffer = new Uint8Array(DISPLAY_BUFFER_SIZE);
let rootDirectoryHandle = null;
let wasmModule = null;
let createModulePromise = null;
let enqueueKey = null;

async function ensureWasmModule() {
  if (wasmModule) return wasmModule;
  if (!createModulePromise) {
    createModulePromise = import("./lava.js")
      .then(({ default: createLavaModule }) => createLavaModule())
      .then((module) => {
        wasmModule = module;
        enqueueKey =
          module.cwrap?.("lava_enqueue_key", "void", ["number"]) ??
          module._lava_enqueue_key;
        
        // 确保WASM模块初始化后就在/app目录下
        // chdir("/app", module);
        
        // 重写FS对象的关键方法，确保只能访问/app目录
        const FS = module.FS;
        if (FS) {
          // 保存原始方法
          const originalChdir = FS.chdir;
          const originalOpen = FS.open;
          const originalWriteFile = FS.writeFile;
          const originalReadFile = FS.readFile;
          const originalUnlink = FS.unlink;
          const originalMkdir = FS.mkdir;
          const originalRmdir = FS.rmdir;
          const originalRename = FS.rename;
          
          // 辅助函数：确保路径在/app目录下
          function ensureAppPath(path) {
            const normalizedPath = path.startsWith("/") ? path : `/${path}`;
            if (normalizedPath.startsWith("/app")) {
              return normalizedPath;
            }
            // 如果路径不是以/app开头，则添加/app前缀
            return `/app${normalizedPath}`;
          }

          FS.chdir = function(path) {
            // 确保路径在/app目录下
            const appPath = ensureAppPath(path);
            return originalChdir.call(this, appPath);
          };
          
          // 重写open方法
          FS.open = function(path, flags, mode) {
            // 确保路径在/app目录下
            const appPath = ensureAppPath(path);
            return originalOpen.call(this, appPath, flags, mode);
          };
          
          // 重写writeFile方法
          FS.writeFile = function(path, data, options) {
            // 确保路径在/app目录下
            const appPath = ensureAppPath(path);
            return originalWriteFile.call(this, appPath, data, options);
          };
          
          // 重写readFile方法
          FS.readFile = function(path, options) {
            // 确保路径在/app目录下
            const appPath = ensureAppPath(path);
            return originalReadFile.call(this, appPath, options);
          };
          
          // 重写unlink方法
          FS.unlink = function(path) {
            // 确保路径在/app目录下
            const appPath = ensureAppPath(path);
            return originalUnlink.call(this, appPath);
          };
          
          // 重写mkdir方法
          FS.mkdir = function(path, mode) {
            // 确保路径在/app目录下
            const appPath = ensureAppPath(path);
            return originalMkdir.call(this, appPath, mode);
          };
          
          // 重写rmdir方法
          FS.rmdir = function(path) {
            // 确保路径在/app目录下
            const appPath = ensureAppPath(path);
            return originalRmdir.call(this, appPath);
          };
          
          // 重写rename方法
          FS.rename = function(oldPath, newPath) {
            // 确保旧路径和新路径都在/app目录下
            const appOldPath = ensureAppPath(oldPath);
            const appNewPath = ensureAppPath(newPath);
            return originalRename.call(this, appOldPath, appNewPath);
          };
        }
        
        console.info("Lava WASM 模块已加载，已限制文件系统访问权限");
        return module;
      })
      .catch((error) => {
        console.warn("加载 WASM 模块失败", error);
        wasmModule = null;
        createModulePromise = null;
        throw error;
      });
  }
  return createModulePromise;
}

function chdir(path, module = wasmModule) {
  try {
    const runtime = module;
    const FS = runtime?.FS;
    if (!FS || typeof FS.chdir !== "function") {
      return;
    }
    
    // 确保路径在/app目录下，如果不是则添加/app前缀
    const normalizedPath = path.startsWith("/") ? path : `/${path}`;
    const appPath = normalizedPath.startsWith("/app") ? normalizedPath : `/app${normalizedPath}`;
    
    FS.chdir(appPath);
  } catch (err) {
    console.warn("chdir error", path, err);
  }
}


const KEY_MAP = {
  ArrowUp: 20,
  ArrowDown: 21,
  ArrowRight: 22,
  ArrowLeft: 23,
  Enter: 13,
  F1: 6,
  F2: 101,
  F3: 102,
  F4: 103,
  F5: 104,
  F6: 105,
  F7: 106,
  F8: 107,
  F9: 108,
  F10: 109,
  F11: 110,
  F12: 111,
  Escape: 27,
  Space: 32,
  Tab: 9,
  Backspace: 8,
  ShiftLeft: 0,
  ShiftRight: 0,
  ControlLeft: 0,
  ControlRight: 0,
  AltLeft: 0,
  AltRight: 0,
};

function mapKeyToCode(code) {
  if (Object.prototype.hasOwnProperty.call(KEY_MAP, code)) {
    return KEY_MAP[code];
  }
  if (code.startsWith("Key") && code.length === 4) {
    return code.toLowerCase().charCodeAt(3);
  }
  switch (code) {
    case "Slash":
      return "/".charCodeAt(0);
    case "Period":
      return ".".charCodeAt(0);
    case "Comma":
      return ",".charCodeAt(0);
    case "BracketLeft":
      return "[".charCodeAt(0);
    case "BracketRight":
      return "]".charCodeAt(0);
    case "Semicolon":
      return ";".charCodeAt(0);
    case "Quote":
      return "'".charCodeAt(0);
    default:
      return 0;
  }
}

// let wasmModule = null;
// let enqueueKey = null;

const wasmExports = {
  bufferPtr: null,
  heapView: null,
  update() {
    if (!wasmModule) return false;

    if (this.bufferPtr === null) {
      const getBufferPtr =
        wasmModule.cwrap?.("lava_display_buffer", "number", []) ??
        wasmModule._lava_display_buffer;
      if (!getBufferPtr) return false;
      this.bufferPtr = getBufferPtr();
    }

    if (!this.heapView) {
      this.heapView = wasmModule.HEAPU8.subarray(
        this.bufferPtr,
        this.bufferPtr + DISPLAY_BUFFER_SIZE
      );
    }

    pixelBuffer.set(this.heapView);
    return true;
  },
};

const fallbackPattern = {
  frame: 0,
  update() {
    const stripeSize = 8;
    for (let y = 0; y < DISPLAY_HEIGHT; y++) {
      for (let xb = 0; xb < DISPLAY_WIDTH / 8; xb++) {
        const baseIndex = y * (DISPLAY_WIDTH / 8) + xb;
        let pattern = 0;
        for (let bit = 0; bit < 8; bit++) {
          const x = xb * 8 + bit;
          const shifted =
            Math.floor((x + this.frame / 4) / stripeSize) +
            Math.floor(y / stripeSize);
          if ((shifted & 1) === 0) {
            pattern |= 1 << bit;
          }
        }
        pixelBuffer[baseIndex] = pattern;
      }
    }
    this.frame += 1;
  },
};

function ensureDirectory(path, module = wasmModule) {
  const runtime = module;
  if (!runtime) return;
  
  // 确保路径在/app目录下，如果不是则添加/app前缀
  const normalized = path.startsWith("/") ? path : `/${path}`;
  const appPath = normalized.startsWith("/app") ? normalized : `/app${normalized}`;
  
  const parts = appPath.split("/").filter(Boolean);
  let current = "/";
  for (const part of parts) {
    const FS = runtime.FS;
    const nextPath = current === "/" ? `/${part}` : `${current}/${part}`;
    
    if (FS?.analyzePath) {
      if (!FS.analyzePath(nextPath).exists) {
        FS.mkdir(nextPath);
      }
    } else if (runtime.FS_createPath) {
      try {
        runtime.FS_createPath(current, part, true, true);
      } catch {
        // 已存在或不支持，忽略即可
      }
    }
    current = nextPath;
  }
}

async function importDirectoryIntoFS(dirHandle, targetPath = "", module = null) {
  const runtime = module ?? (await ensureWasmModule());
  const FS = runtime?.FS;
  if (!FS) {
    console.warn("WASM FS 未初始化，无法导入目录");
    return;
  }
  
  // 确保目标路径在/app目录下，如果不是则添加/app前缀
  const normalizedTargetPath = targetPath.startsWith("/") ? targetPath : `/${targetPath}`;
  const appPath = normalizedTargetPath.startsWith("/app") ? normalizedTargetPath : `/app${normalizedTargetPath}`;
  
  if (appPath) ensureDirectory(appPath, runtime);
  const basePath = appPath
    ? appPath.startsWith("/")
      ? appPath
      : `/${appPath}`
    : "";
  for await (const entry of dirHandle.values()) {
    if(entry.name.startsWith(".DS_")) continue;
    if(entry.name.length > 16) continue;
    const entryPath = `${basePath}/${entry.name}`.replace(/\/+/g, "/");
    
    if (entry.kind === "file") {
      const file = await entry.getFile();
      const buffer = await file.arrayBuffer();
      const data = new Uint8Array(buffer);
      if (FS.analyzePath?.(entryPath)?.exists) {
        FS.unlink(entryPath);
      } else {
        try {
          FS.unlink(entryPath);
        } catch {
          // 文件不存在时会抛出异常，忽略即可
        }
      }
      FS.writeFile(entryPath, data, { canOwn: true });
      console.info("已导入文件:", entryPath, data.length, "bytes");
    } else if (entry.kind === "directory") {
      ensureDirectory(entryPath, runtime);
      await importDirectoryIntoFS(entry, entryPath, runtime);
    }
  }
}

async function getFileHandleFromRoot(relativePath, create = false) {
  if (!rootDirectoryHandle) return null;
  
  // 确保相对路径不会访问到上级目录
  const sanitizedPath = relativePath.replace(/\.\./g, "");
  const parts = sanitizedPath.split("/").filter(Boolean);
  
  let current = rootDirectoryHandle;
  for (let i = 0; i < parts.length; i++) {
    const name = parts[i];
    const isFile = i === parts.length - 1;
    if (isFile) {
      return await current.getFileHandle(name, { create });
    }
    current = await current.getDirectoryHandle(name, { create });
  }
  return current;
}

async function syncFileToDisk(path) {
  if (!wasmModule?.FS || !rootDirectoryHandle) return;
  
  // 确保路径在/app目录下，如果不是则添加/app前缀
  const normalizedPath = path.startsWith("/") ? path : `/${path}`;
  const appPath = normalizedPath.startsWith("/app") ? normalizedPath : `/app${normalizedPath}`;
  
  const normalized = appPath.replace(/^\/+/, "");
  const fsPath = appPath;
  try {
    const data = wasmModule.FS.readFile(fsPath, { encoding: "binary" });
    const fileHandle = await getFileHandleFromRoot(normalized, true);
    if (!fileHandle) return;
    const writable = await fileHandle.createWritable();
    await writable.write(data);
    await writable.close();
    console.info("已同步文件到磁盘:", normalized);
  } catch (error) {
    console.warn("同步文件失败", path, error);
  }
}

async function restartVm(basePath) {
  try {
    const module = await ensureWasmModule();
    
    // 确保基础路径在/app目录下，如果不是则添加/app前缀
    if (basePath) {
      const normalizedPath = basePath.startsWith("/") ? basePath : `/${basePath}`;
      const appPath = normalizedPath.startsWith("/app") ? normalizedPath : `/app${normalizedPath}`;
      module.ccall("lvm_set_base_path", "void", ["string"], [appPath]);
    }
    await module.ccall(
      "lvm_request_restart",
      "void",
      [],
      [],
      { async: true }
    );
  } catch (error) {
    console.warn("重启 Lava VM 失败", error);
  }
}

function drawFrame() {
  const data = imageData.data;
  let di = 0;

  for (let y = 0; y < DISPLAY_HEIGHT; y++) {
    for (let xb = 0; xb < DISPLAY_WIDTH / 8; xb++) {
      const byte = pixelBuffer[y * (DISPLAY_WIDTH / 8) + xb];
      // 🔸 MSB-first，从bit7开始
      for (let bit = 7; bit >= 0; bit--) {
        const on = (byte >> bit) & 1;
        const shade = on ? 20 : 230;
        data[di++] = shade;
        data[di++] = shade;
        data[di++] = shade;
        data[di++] = 255;
      }
    }
  }

  ctx.putImageData(imageData, 0, 0);
}
function setScale(scale) {
  const clamped = Math.max(1, Math.min(8, Number(scale) || 1));
  canvas.style.width = `${DISPLAY_WIDTH * clamped}px`;
  canvas.style.height = `${DISPLAY_HEIGHT * clamped}px`;
  scaleValue.textContent = `${clamped}×`;
}

function handleScaleInput(event) {
  setScale(event.target.value);
}

function handleKeyboardClick(event) {
  const button = event.target.closest("button[data-key]");
  if (!button) return;
  triggerKey(button.dataset.key);
  flashButton(button);
}

function handleKeyboardEvent(event) {
  const selector = `.keyboard-row button[data-key="${event.code}"]`;
  const button = keyboardPanel.querySelector(selector);
  if (button) {
    if (event.type === "keydown") {
      flashButton(button);
      triggerKey(event.code);
      event.preventDefault();
    }
  }
}

function flashButton(button) {
  button.classList.add("active");
  clearTimeout(button.dataset.timeout);
  button.dataset.timeout = setTimeout(
    () => button.classList.remove("active"),
    120
  );
}

const keyboardListeners = new Set();

function triggerKey(code) {
  
  const nativeCode = mapKeyToCode(code);
  // console.log("触发按键:", code, nativeCode);
  if (!nativeCode) return;
  const payload = { code, nativeCode, timestamp: performance.now() };
  for (const listener of keyboardListeners) {
    try {
      listener(payload);
    } catch (err) {
      console.error("keyboard listener error", err);
    }
  }
}

keyboardListeners.add((payload) => {
  if (enqueueKey) {
    enqueueKey(payload.nativeCode);
  }
  console.debug("虚拟按键:", payload.code);
});

scaleInput.addEventListener("input", handleScaleInput);
keyboardPanel.addEventListener("click", handleKeyboardClick);
document.addEventListener("keydown", handleKeyboardEvent);

if (fsButton) {
  fsButton.addEventListener("click", async () => {
    if (!window.showDirectoryPicker) {
      alert("当前浏览器不支持 File System Access API。");
      return;
    }
    // 先选择目录，再加载WASM模块
    try {
      const handle = await window.showDirectoryPicker();
      rootDirectoryHandle = handle;
      
      // 显示加载状态
      fsButton.textContent = "加载中...";
      fsButton.disabled = true;
      
      // 在选择目录后再加载WASM模块
      const module = await ensureWasmModule();
      const FS = module?.FS;
      if (!FS && !module?.FS_createPath) {
        alert("WASM 虚拟文件系统尚未就绪，请稍后重试。");
        fsButton.textContent = "选择程序目录";
        fsButton.disabled = false;
        return;
      }
      
      // 创建/app目录并切换到该目录
      ensureDirectory("/app", module);
      // chdir("/app", module);
      
      // 只导入到/app目录下
      await importDirectoryIntoFS(handle, "/app", module);
      
      // 恢复按钮状态
      fsButton.textContent = "重新选择目录";
      fsButton.disabled = false;
      
      console.log("目录已载入虚拟文件系统");
      chdir("/app", module);
      await restartVm("/app");
    } catch (error) {
      console.warn("目录授权或读取失败", error);
      fsButton.textContent = "选择程序目录";
      fsButton.disabled = false;
    }
  });
}

if (screenshotButton) {
  screenshotButton.addEventListener("click", async () => {
    try {
      const module = await ensureWasmModule();
      module.ccall("PrtScr_All", "void", [], []);
      alert("已截取当前画面");
    } catch (error) {
      console.warn("截屏失败", error);
    }
  });
}

setScale(scaleInput.value);

function frameLoop() {
  if (!wasmExports.update()) {
    fallbackPattern.update();
  }
  drawFrame();
  requestAnimationFrame(frameLoop);
}

// 移除页面加载时的WASM模块初始化
// await ensureWasmModule();
requestAnimationFrame(frameLoop);
