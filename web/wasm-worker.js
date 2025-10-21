// Web Worker for running WASM module to prevent UI blocking

const DISPLAY_WIDTH = 160;
const DISPLAY_HEIGHT = 80;
const DISPLAY_BUFFER_SIZE = (DISPLAY_WIDTH * DISPLAY_HEIGHT) / 8;

let wasmModule = null;
let createModulePromise = null;
let enqueueKey = null;
let bufferPtr = null;
let heapView = null;
let isRunning = false;

// Import and initialize WASM module
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
        
        self.postMessage({ type: 'wasmLoaded' });
        console.info("Lava WASM 模块已加载，已限制文件系统访问权限");
        return module;
      })
      .catch((error) => {
        console.warn("加载 WASM 模块失败", error);
        wasmModule = null;
        createModulePromise = null;
        self.postMessage({ type: 'wasmError', error: error.message });
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

// 从文件数据创建文件系统
function createFilesystemFromData(fileData, targetPath = "/app") {
  const module = wasmModule;
  if (!module) return;
  
  const FS = module.FS;
  if (!FS) {
    console.warn("WASM FS 未初始化，无法创建文件系统");
    return;
  }
  
  // 确保目标路径在/app目录下，如果不是则添加/app前缀
  const normalizedTargetPath = targetPath.startsWith("/") ? targetPath : `/${targetPath}`;
  const appPath = normalizedTargetPath.startsWith("/app") ? normalizedTargetPath : `/app${normalizedTargetPath}`;
  
  if (appPath) ensureDirectory(appPath, module);
  
  // 处理文件数据
  fileData.forEach(file => {
    const filePath = `${appPath}/${file.name}`.replace(/\/+/g, "/");
    
    // 确保目录存在
    const dirPath = filePath.substring(0, filePath.lastIndexOf('/'));
    if (dirPath && dirPath !== appPath) {
      ensureDirectory(dirPath, module);
    }
    
    // 写入文件
    if (FS.analyzePath?.(filePath)?.exists) {
      FS.unlink(filePath);
    } else {
      try {
        FS.unlink(filePath);
      } catch {
        // 文件不存在时会抛出异常，忽略即可
      }
    }
    FS.writeFile(filePath, file.data, { canOwn: true });
    console.info("已创建文件:", filePath, file.data.length, "bytes");
  });
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
    self.postMessage({ type: 'vmRestarted' });
  } catch (error) {
    console.warn("重启 Lava VM 失败", error);
    self.postMessage({ type: 'vmError', error: error.message });
  }
}

function updateDisplayBuffer() {
  if (!wasmModule) return false;

  if (bufferPtr === null) {
    const getBufferPtr =
      wasmModule.cwrap?.("lava_display_buffer", "number", []) ??
      wasmModule._lava_display_buffer;
    if (!getBufferPtr) return false;
    bufferPtr = getBufferPtr();
  }

  if (!heapView) {
    heapView = wasmModule.HEAPU8.subarray(
      bufferPtr,
      bufferPtr + DISPLAY_BUFFER_SIZE
    );
  }

  // 将显示缓冲区数据发送到主线程
  const pixelBuffer = new Uint8Array(DISPLAY_BUFFER_SIZE);
  pixelBuffer.set(heapView);
  self.postMessage({ 
    type: 'displayUpdate', 
    pixelBuffer: pixelBuffer 
  });
  
  return true;
}

// 处理来自主线程的消息
self.addEventListener('message', async (event) => {
  const { type, data } = event.data;
  
  switch (type) {
    case 'init':
      // 初始化WASM模块
      try {
        await ensureWasmModule();
        isRunning = true;
        runFrameLoop();
      } catch (error) {
        self.postMessage({ type: 'wasmError', error: error.message });
      }
      break;
      
    case 'createFilesystem':
      // 从文件数据创建文件系统
      try {
        createFilesystemFromData(data.fileData, data.targetPath);
        self.postMessage({ type: 'filesystemCreated' });
      } catch (error) {
        self.postMessage({ type: 'filesystemError', error: error.message });
      }
      break;
      
    case 'restart':
      // 重启VM
      try {
        await restartVm(data.basePath);
      } catch (error) {
        self.postMessage({ type: 'vmError', error: error.message });
      }
      break;
      
    case 'keyPress':
      // 处理按键事件
      if (enqueueKey) {
        enqueueKey(data.nativeCode);
      }
      break;
      
    case 'screenshot':
      // 截屏
      try {
        const module = await ensureWasmModule();
        module.ccall("PrtScr_All", "void", [], []);
        self.postMessage({ type: 'screenshotTaken' });
      } catch (error) {
        self.postMessage({ type: 'screenshotError', error: error.message });
      }
      break;
      
    case 'stop':
      // 停止运行循环
      isRunning = false;
      break;
  }
});

// 运行帧循环
function runFrameLoop() {
  if (!isRunning) return;
  
  // 更新显示缓冲区
  if (!updateDisplayBuffer()) {
    // 如果WASM模块不可用，发送后备模式
    self.postMessage({ type: 'fallbackMode' });
  }
  
  // 继续下一帧
  setTimeout(() => runFrameLoop(), 16); // 约60fps
}