/**
 * Shared Form Controls — reusable form elements for command views.
 *
 * Creates select dropdowns, toggle switches, text inputs, and radio groups
 * using the dashboard's CSS classes.
 */
export function createSelect(labelText, options) {
    const wrapper = document.createElement('div');
    wrapper.className = 'form-group';
    const label = document.createElement('label');
    label.className = 'form-label';
    label.textContent = labelText;
    wrapper.appendChild(label);
    const select = document.createElement('select');
    select.className = 'form-select';
    for (const opt of options) {
        const o = document.createElement('option');
        o.value = opt.value;
        o.textContent = opt.label;
        select.appendChild(o);
    }
    wrapper.appendChild(select);
    return {
        wrapper,
        getValue: () => select.value,
        setValue: (v) => { select.value = v; },
        onChange: (cb) => {
            select.addEventListener('change', () => cb(select.value));
        },
    };
}
export function createToggle(name, labelText) {
    const wrapper = document.createElement('div');
    wrapper.style.cssText = 'display: flex; align-items: center; gap: var(--space-2);';
    const toggle = document.createElement('label');
    toggle.className = 'toggle';
    const input = document.createElement('input');
    input.type = 'checkbox';
    input.id = `ctrl-${name}`;
    const slider = document.createElement('span');
    slider.className = 'toggle-slider';
    toggle.appendChild(input);
    toggle.appendChild(slider);
    wrapper.appendChild(toggle);
    const label = document.createElement('span');
    label.className = 'form-label';
    label.style.marginBottom = '0';
    label.textContent = labelText;
    wrapper.appendChild(label);
    return {
        wrapper,
        name,
        isChecked: () => input.checked,
        setChecked: (v) => { input.checked = v; },
        onChange: (cb) => {
            input.addEventListener('change', () => cb(input.checked));
        },
    };
}
export function createTextInput(name, labelText, placeholder) {
    const wrapper = document.createElement('div');
    wrapper.className = 'form-group';
    const label = document.createElement('label');
    label.className = 'form-label';
    label.textContent = labelText;
    label.htmlFor = `ctrl-${name}`;
    wrapper.appendChild(label);
    const input = document.createElement('input');
    input.className = 'form-input';
    input.type = 'text';
    input.id = `ctrl-${name}`;
    input.placeholder = placeholder ?? labelText;
    wrapper.appendChild(input);
    return {
        wrapper,
        getValue: () => input.value.trim(),
        setValue: (v) => { input.value = v; },
        onChange: (cb) => {
            input.addEventListener('change', () => cb(input.value.trim()));
        },
    };
}
export function createRadioGroup(name, labelText, options) {
    const wrapper = document.createElement('div');
    wrapper.className = 'form-group';
    const label = document.createElement('label');
    label.className = 'form-label';
    label.textContent = labelText;
    wrapper.appendChild(label);
    const group = document.createElement('div');
    group.style.cssText = 'display: flex; flex-direction: column; gap: var(--space-2);';
    const callbacks = [];
    for (let i = 0; i < options.length; i++) {
        const opt = options[i];
        const row = document.createElement('label');
        row.style.cssText = 'display: flex; align-items: center; gap: var(--space-2); cursor: pointer;';
        const radio = document.createElement('input');
        radio.type = 'radio';
        radio.name = `radio-${name}`;
        radio.value = opt.value;
        if (i === 0)
            radio.checked = true;
        radio.addEventListener('change', () => {
            if (radio.checked)
                callbacks.forEach((cb) => cb(opt.value));
        });
        row.appendChild(radio);
        const text = document.createElement('span');
        text.style.cssText = 'font-size: var(--text-sm);';
        text.textContent = opt.label;
        row.appendChild(text);
        if (opt.description) {
            const desc = document.createElement('span');
            desc.style.cssText = 'font-size: var(--text-xs); color: var(--color-text-muted); margin-left: var(--space-1);';
            desc.textContent = `— ${opt.description}`;
            row.appendChild(desc);
        }
        group.appendChild(row);
    }
    wrapper.appendChild(group);
    return {
        wrapper,
        getValue: () => {
            const checked = group.querySelector(`input[name="radio-${name}"]:checked`);
            return checked?.value ?? options[0]?.value ?? '';
        },
        setValue: (v) => {
            const radio = group.querySelector(`input[value="${v}"]`);
            if (radio)
                radio.checked = true;
        },
        onChange: (cb) => { callbacks.push(cb); },
    };
}
export function createCheckboxGroup(name, labelText, options) {
    const wrapper = document.createElement('div');
    wrapper.className = 'form-group';
    const label = document.createElement('label');
    label.className = 'form-label';
    label.textContent = labelText;
    wrapper.appendChild(label);
    const group = document.createElement('div');
    group.style.cssText = 'display: flex; flex-wrap: wrap; gap: var(--space-3);';
    const checkboxes = [];
    for (const opt of options) {
        const row = document.createElement('label');
        row.style.cssText = 'display: flex; align-items: center; gap: var(--space-1); cursor: pointer; font-size: var(--text-sm);';
        const cb = document.createElement('input');
        cb.type = 'checkbox';
        cb.name = `cb-${name}`;
        cb.value = opt.value;
        row.appendChild(cb);
        row.appendChild(document.createTextNode(opt.label));
        checkboxes.push({ input: cb, value: opt.value });
        group.appendChild(row);
    }
    wrapper.appendChild(group);
    return {
        wrapper,
        getSelected: () => checkboxes.filter((c) => c.input.checked).map((c) => c.value),
        setSelected: (values) => {
            for (const c of checkboxes) {
                c.input.checked = values.includes(c.value);
            }
        },
    };
}
