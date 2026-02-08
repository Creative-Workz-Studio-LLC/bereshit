/**
 * Document Types Editor — §16 build.config.yaml
 *
 * Table/repeater for document type → register → theme mappings.
 */
import { configInput } from './editor-base.js';
export const docTypesEditor = {
    sectionKey: 'document_types',
    title: 'Document Types',
    description: 'Document type → register → theme mappings.',
    buildForm(data) {
        const d = (data ?? {});
        const el = document.createElement('div');
        const rows = [];
        const table = document.createElement('div');
        table.style.cssText = 'display: flex; flex-direction: column; gap: var(--space-3);';
        el.appendChild(table);
        function addRow(entry) {
            const row = document.createElement('div');
            row.className = 'card';
            row.style.cssText = 'display: grid; grid-template-columns: 1fr 1fr 1fr; gap: var(--space-3);';
            const type = configInput('Type', entry.type, { placeholder: 'e.g., eula' });
            const register = configInput('Register', entry.register, { placeholder: 'e.g., formal' });
            const theme = configInput('Theme', entry.theme, { placeholder: 'e.g., legal' });
            const rationale = configInput('Rationale', entry.rationale);
            const removeBtn = document.createElement('button');
            removeBtn.className = 'btn btn-danger';
            removeBtn.textContent = 'Remove';
            removeBtn.style.cssText = 'grid-column: 1 / -1; justify-self: end;';
            removeBtn.addEventListener('click', () => {
                const idx = rows.findIndex((r) => r.row === row);
                if (idx >= 0)
                    rows.splice(idx, 1);
                row.remove();
                el.dispatchEvent(new Event('change', { bubbles: true }));
            });
            row.appendChild(type.wrapper);
            row.appendChild(register.wrapper);
            row.appendChild(theme.wrapper);
            rationale.wrapper.style.gridColumn = '1 / -1';
            row.appendChild(rationale.wrapper);
            row.appendChild(removeBtn);
            table.appendChild(row);
            rows.push({ type, register, theme, rationale, row });
        }
        for (const entry of d.types ?? []) {
            addRow(entry);
        }
        const addBtn = document.createElement('button');
        addBtn.className = 'btn';
        addBtn.textContent = '+ Add Document Type';
        addBtn.style.marginTop = 'var(--space-3)';
        addBtn.addEventListener('click', () => {
            addRow({ type: '', register: '', theme: '', rationale: '' });
            el.dispatchEvent(new Event('change', { bubbles: true }));
        });
        el.appendChild(addBtn);
        return {
            element: el,
            getData: () => ({
                types: rows.map((r) => ({
                    type: r.type.getValue(),
                    register: r.register.getValue(),
                    theme: r.theme.getValue(),
                    rationale: r.rationale.getValue(),
                })),
            }),
        };
    },
};
