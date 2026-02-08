/**
 * Company Conventions Editor — §18 build.config.yaml
 *
 * Board members, legal name, contact emails, name variants.
 */
import { configInput, configStringArray, configKeyValue, configSection } from './editor-base.js';
export const companyEditor = {
    sectionKey: 'company_conventions',
    title: 'Company',
    description: 'Company identity, board members, and contact information.',
    buildForm(data) {
        const d = (data ?? {});
        const el = document.createElement('div');
        // ── Names ─────────────────────────────────────────────────────
        const legalName = configInput('Legal Name', d.legal_name ?? '', { placeholder: 'CreativeWorkzStudio LLC' });
        const attrName = configInput('Attribute Name', d.attribute_name ?? '', { placeholder: '{company-name}' });
        el.appendChild(legalName.wrapper);
        el.appendChild(attrName.wrapper);
        const variants = configStringArray('Name Variants', d.raw_name_variants ?? [], {
            placeholder: 'e.g., CWS, Creative Workz Studio',
        });
        el.appendChild(variants.wrapper);
        // ── Board Members ─────────────────────────────────────────────
        el.appendChild(configSection('Board Members'));
        const memberContainer = document.createElement('div');
        memberContainer.style.cssText = 'display: flex; flex-direction: column; gap: var(--space-3);';
        el.appendChild(memberContainer);
        const memberRows = [];
        function addMember(member) {
            const row = document.createElement('div');
            row.className = 'card';
            row.style.cssText = 'display: grid; grid-template-columns: 1fr 1fr 1fr auto; gap: var(--space-3); align-items: end;';
            const name = configInput('Name', member.name);
            const title = configInput('Title', member.title, { placeholder: 'e.g., CEO' });
            const role = configInput('Role', member.role, { placeholder: 'e.g., Founder & CEO' });
            const removeBtn = document.createElement('button');
            removeBtn.className = 'btn btn-danger';
            removeBtn.textContent = 'X';
            removeBtn.style.marginBottom = 'var(--space-4)';
            removeBtn.addEventListener('click', () => {
                const idx = memberRows.findIndex((r) => r.row === row);
                if (idx >= 0)
                    memberRows.splice(idx, 1);
                row.remove();
                el.dispatchEvent(new Event('change', { bubbles: true }));
            });
            row.appendChild(name.wrapper);
            row.appendChild(title.wrapper);
            row.appendChild(role.wrapper);
            row.appendChild(removeBtn);
            memberContainer.appendChild(row);
            memberRows.push({ name, title, role, row });
        }
        for (const m of d.board_members ?? []) {
            addMember(m);
        }
        const addMemberBtn = document.createElement('button');
        addMemberBtn.className = 'btn';
        addMemberBtn.textContent = '+ Add Board Member';
        addMemberBtn.style.marginTop = 'var(--space-2)';
        addMemberBtn.addEventListener('click', () => {
            addMember({ name: '', title: '', role: '' });
            el.dispatchEvent(new Event('change', { bubbles: true }));
        });
        el.appendChild(addMemberBtn);
        // ── Contact Emails ────────────────────────────────────────────
        el.appendChild(configSection('Contact Emails'));
        const emails = configKeyValue('Emails', d.contact_emails ?? {});
        el.appendChild(emails.wrapper);
        // ── Reading Guide Parties ─────────────────────────────────────
        el.appendChild(configSection('Reading Guide'));
        const parties = configStringArray('Required Parties', d.reading_guide_parties ?? [], {
            placeholder: 'e.g., Board of Directors',
        });
        el.appendChild(parties.wrapper);
        return {
            element: el,
            getData: () => ({
                legal_name: legalName.getValue(),
                attribute_name: attrName.getValue(),
                raw_name_variants: variants.getValue(),
                board_members: memberRows.map((r) => ({
                    name: r.name.getValue(),
                    title: r.title.getValue(),
                    role: r.role.getValue(),
                })),
                contact_emails: emails.getValue(),
                reading_guide_parties: parties.getValue(),
            }),
        };
    },
};
