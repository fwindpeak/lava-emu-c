const DISPLAY_WIDTH = 160;
const DISPLAY_HEIGHT = 80;
const DISPLAY_BUFFER_SIZE = (DISPLAY_WIDTH * DISPLAY_HEIGHT) / 8;

const canvas = document.getElementById("lava-canvas");
const ctx = canvas.getContext("2d", { alpha: false });
const scaleInput = document.getElementById("scale-input");
const scaleValue = document.getElementById("scale-value");
const keyboardPanel = document.querySelector(".keyboard");

const imageData = ctx.createImageData(DISPLAY_WIDTH, DISPLAY_HEIGHT);
const pixelBuffer = new Uint8Array(DISPLAY_BUFFER_SIZE);

let wasmModule = null;

try {
  const { default: createLavaModule } = await import("./lava.js");
  wasmModule = await createLavaModule();
  console.info("Lava WASM 模块已加载");
} catch (error) {
  console.warn("未能加载 WASM 模块，使用演示模式。", error);
}

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

    const fillDemo =
      wasmModule.cwrap?.("lava_display_fill_demo", "void", ["number"]) ??
      wasmModule._lava_display_fill_demo;
    if (fillDemo) {
      fillDemo(performance.now() / 16);
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

function drawFrame() {
  const data = imageData.data;
  let di = 0;
  for (let y = 0; y < DISPLAY_HEIGHT; y++) {
    for (let xb = 0; xb < DISPLAY_WIDTH / 8; xb++) {
      const byte = pixelBuffer[y * (DISPLAY_WIDTH / 8) + xb];
      for (let bit = 0; bit < 8; bit++) {
        const on = (byte >> bit) & 1;
        const shade = on ? 230 : 20;
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
  const payload = { code, timestamp: performance.now() };
  for (const listener of keyboardListeners) {
    try {
      listener(payload);
    } catch (err) {
      console.error("keyboard listener error", err);
    }
  }
}

keyboardListeners.add((payload) => {
  if (wasmModule?.ccall) {
    // 预留：将来可以通过 Module.ccall 将按键传递给虚拟机。
  }
  console.debug("虚拟按键:", payload.code);
});

scaleInput.addEventListener("input", handleScaleInput);
keyboardPanel.addEventListener("click", handleKeyboardClick);
document.addEventListener("keydown", handleKeyboardEvent);

setScale(scaleInput.value);

function frameLoop() {
  if (!wasmExports.update()) {
    fallbackPattern.update();
  }
  drawFrame();
  requestAnimationFrame(frameLoop);
}

requestAnimationFrame(frameLoop);
