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
const restartButton = document.getElementById("restart-button");

const imageData = ctx.createImageData(DISPLAY_WIDTH, DISPLAY_HEIGHT);
const pixelBuffer = new Uint8Array(DISPLAY_BUFFER_SIZE);
let rootDirectoryHandle = null;
let wasmWorker = null;
let workerReady = false;

// 初始化Web Worker
function initWorker() {
  wasmWorker = new Worker('./wasm-worker.js');
  
  // 处理来自Worker的消息
  wasmWorker.addEventListener('message', (event) => {
    const { type, data, error } = event.data;
    
    switch (type) {
      case 'wasmLoaded':
        console.log('WASM模块在Worker中加载成功');
        workerReady = true;
        break;
        
      case 'wasmError':
        console.error('WASM模块加载失败:', error);
        // alert('加载WASM模块失败: ' + error);
        break;
        
      case 'displayUpdate':
        // 更新显示缓冲区
        // displayUpdate消息直接在event.data中包含pixelBuffer
        pixelBuffer.set(event.data.pixelBuffer);
        break;
        
      case 'fallbackMode':
        // WASM不可用，使用后备模式
        fallbackPattern.update();
        break;
        
      case 'filesystemCreated':
        console.log('文件系统已创建');
        // 恢复按钮状态
        fsButton.textContent = "重新选择目录";
        fsButton.disabled = false;
        // 重启VM
        wasmWorker.postMessage({ type: 'restart', data: { basePath: '/app' } });
        break;
        
      case 'filesystemError':
        console.error('创建文件系统失败:', error);
        alert('创建文件系统失败: ' + error);
        fsButton.textContent = "选择程序目录";
        fsButton.disabled = false;
        break;
        
      case 'vmRestarted':
        console.log('VM已重启');
        break;
        
      case 'vmError':
        console.error('VM重启失败:', error);
        alert('VM重启失败: ' + error);
        break;
        
      case 'screenshotTaken':
        alert('已截取当前画面');
        break;
        
      case 'screenshotError':
        console.error('截屏失败:', error);
        alert('截屏失败: ' + error);
        break;
    }
  });
  
  // 初始化WASM模块
  wasmWorker.postMessage({ type: 'init' });
}

// 递归读取目录中的所有文件
async function readDirectoryFiles(dirHandle, path = '') {
  const files = [];
  
  for await (const entry of dirHandle.values()) {
    // 跳过隐藏文件和过长的文件名
    if (entry.name.startsWith('.DS_')) continue;
    if (entry.name.length > 16) continue;
    
    const entryPath = path ? `${path}/${entry.name}` : entry.name;
    
    if (entry.kind === 'file') {
      const file = await entry.getFile();
      const buffer = await file.arrayBuffer();
      const data = new Uint8Array(buffer);
      
      files.push({
        name: entryPath,
        data: data
      });
    } else if (entry.kind === 'directory') {
      // 递归读取子目录
      const subFiles = await readDirectoryFiles(entry, entryPath);
      files.push(...subFiles);
    }
  }
  
  return files;
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
  if (!nativeCode) return;
  
  // 发送按键事件到Worker
  if (wasmWorker && workerReady) {
    wasmWorker.postMessage({ 
      type: 'keyPress', 
      data: { code, nativeCode } 
    });
  }
  
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
    
    try {
      const handle = await window.showDirectoryPicker();
      rootDirectoryHandle = handle;
      
      // 显示加载状态
      fsButton.textContent = "加载中...";
      fsButton.disabled = true;
      
      // 确保Worker已初始化
      if (!wasmWorker) {
        initWorker();
      }
      
      // 等待Worker准备就绪
      if (!workerReady) {
        // 等待WASM加载完成
        await new Promise((resolve) => {
          const checkReady = () => {
            if (workerReady) resolve();
            else setTimeout(checkReady, 100);
          };
          checkReady();
        });
      }
      
      // 读取目录中的所有文件
      console.log("正在读取目录文件...");
      const fileData = await readDirectoryFiles(handle);
      console.log(`已读取 ${fileData.length} 个文件`);
      
      // 发送文件数据到Worker
      wasmWorker.postMessage({ 
        type: 'createFilesystem', 
        data: { fileData, targetPath: '/app' } 
      });
      
      console.log("正在创建虚拟文件系统");
    } catch (error) {
      console.warn("目录授权或读取失败", error);
      fsButton.textContent = "选择程序目录";
      fsButton.disabled = false;
    }
  });
}

if (screenshotButton) {
  screenshotButton.addEventListener("click", async () => {
    if (!wasmWorker || !workerReady) {
      alert("WASM模块尚未加载完成");
      return;
    }
    
    wasmWorker.postMessage({ type: 'screenshot' });
  });
}

if (restartButton) {
  restartButton.addEventListener("click", async () => {
    if (wasmWorker && workerReady) {
      wasmWorker.postMessage({ type: 'restart', data: { basePath: '/app' } });
    }
    console.log("点击重新启动按钮");
    setTimeout(() => {
      location.reload();
    }, 1000);
  });
}

setScale(scaleInput.value);

// 初始化Worker
initWorker();

// 帧循环
function frameLoop() {
  drawFrame();
  requestAnimationFrame(frameLoop);
}

requestAnimationFrame(frameLoop);
