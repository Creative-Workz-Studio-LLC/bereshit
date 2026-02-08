/**
 * File Picker — unified file selection with three input methods:
 *
 * 1. Native file dialog (Tauri mode) — click "Browse" button
 * 2. Drag-and-drop — drop file onto the drop zone
 * 3. Typed path — manual text input
 *
 * In web mode, the browse button uses <input type="file"> as fallback.
 */

import { getAdapter } from '../../scripts/adapter.js';

export interface FilePickerOptions {
  /** Label for the form group. */
  label: string;
  /** Placeholder text for the input. */
  placeholder?: string;
  /** File extension filters (e.g., [{ name: 'AsciiDoc', extensions: ['adoc'] }]). */
  filters?: Array<{ name: string; extensions: string[] }>;
  /** Allow directory selection instead of files. */
  directory?: boolean;
  /** Allow multiple file selection. */
  multiple?: boolean;
  /** Called when a file is selected. */
  onChange?: (paths: string[]) => void;
}

export interface FilePicker {
  /** The container element to mount in the DOM. */
  element: HTMLElement;
  /** Get the current file path(s). */
  getValue(): string;
  /** Get all selected paths. */
  getPaths(): string[];
  /** Set the value programmatically. */
  setValue(path: string): void;
}

export function createFilePicker(opts: FilePickerOptions): FilePicker {
  const adapter = getAdapter();
  let selectedPaths: string[] = [];

  // Container
  const wrapper = document.createElement('div');
  wrapper.className = 'form-group';

  // Label
  const label = document.createElement('label');
  label.className = 'form-label';
  label.textContent = opts.label;
  wrapper.appendChild(label);

  // Input row (text input + browse button)
  const inputRow = document.createElement('div');
  inputRow.className = 'file-picker';

  const input = document.createElement('input');
  input.className = 'form-input file-picker-input';
  input.type = 'text';
  input.placeholder = opts.placeholder ?? 'Enter path or drag & drop...';
  input.addEventListener('change', () => {
    selectedPaths = [input.value.trim()];
    opts.onChange?.(selectedPaths);
  });
  inputRow.appendChild(input);

  const browseBtn = document.createElement('button');
  browseBtn.className = 'btn file-picker-btn';
  browseBtn.textContent = 'Browse';
  browseBtn.type = 'button';

  if (adapter.getMode() === 'tauri') {
    // Tauri: native file dialog.
    browseBtn.addEventListener('click', async () => {
      const paths = await adapter.openFileDialog({
        title: opts.label,
        filters: opts.filters,
        directory: opts.directory,
        multiple: opts.multiple,
      });
      if (paths.length > 0) {
        selectedPaths = paths;
        input.value = paths.join(', ');
        opts.onChange?.(paths);
      }
    });
  } else {
    // Web: <input type="file"> fallback.
    const fileInput = document.createElement('input');
    fileInput.type = 'file';
    fileInput.style.display = 'none';
    if (opts.multiple) fileInput.multiple = true;
    if (opts.filters) {
      fileInput.accept = opts.filters
        .flatMap((f) => f.extensions.map((e) => `.${e}`))
        .join(',');
    }
    fileInput.addEventListener('change', () => {
      if (fileInput.files && fileInput.files.length > 0) {
        // In web mode, we get File objects, not paths.
        // Use the file name as a hint — the server resolves paths.
        const names = Array.from(fileInput.files).map((f) => f.name);
        selectedPaths = names;
        input.value = names.join(', ');
        opts.onChange?.(names);
      }
    });
    wrapper.appendChild(fileInput);
    browseBtn.addEventListener('click', () => fileInput.click());
  }

  inputRow.appendChild(browseBtn);
  wrapper.appendChild(inputRow);

  // Drop zone
  const dropZone = document.createElement('div');
  dropZone.className = 'file-picker-dropzone';
  dropZone.textContent = 'Drop file here';

  dropZone.addEventListener('dragover', (e) => {
    e.preventDefault();
    dropZone.classList.add('active');
  });

  dropZone.addEventListener('dragleave', () => {
    dropZone.classList.remove('active');
  });

  dropZone.addEventListener('drop', (e) => {
    e.preventDefault();
    dropZone.classList.remove('active');
    if (e.dataTransfer?.files && e.dataTransfer.files.length > 0) {
      const names = Array.from(e.dataTransfer.files).map((f) => f.name);
      selectedPaths = names;
      input.value = names.join(', ');
      opts.onChange?.(names);
    }
  });

  wrapper.appendChild(dropZone);

  return {
    element: wrapper,
    getValue: () => input.value.trim(),
    getPaths: () => selectedPaths,
    setValue: (path: string) => {
      input.value = path;
      selectedPaths = [path];
    },
  };
}
