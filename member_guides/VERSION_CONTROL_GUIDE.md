# Do You Need Version Control (Git)?

## Short answer
**Yes — strongly recommended, but not strictly required.**

For a 5-person project where everyone edits code that gets merged, Git will save you
from a lot of pain. But because of how we structured this project (each member edits
only ONE file), you *can* survive without it. Here's the honest trade-off.

---

## Option A — Use Git + GitHub (Recommended)

### Why it's worth it
- Everyone works at the same time without overwriting each other
- Full history — if someone breaks something, you roll back instantly
- No "final_v2_FINAL_real.zip" chaos over WhatsApp
- Looks professional if you submit the repo link
- Since each member owns a separate file, **merge conflicts will be almost zero**

### Effort
About 30 minutes for the whole team to set up once. After that it's 3 commands.

### Quick setup
```bash
# Member 1 (once):
cd cg_project
git init
git add .
git commit -m "Initial project skeleton"
# Create a repo on github.com, then:
git remote add origin https://github.com/YOURNAME/cg_project.git
git push -u origin main
```

```bash
# Every other member (once):
git clone https://github.com/YOURNAME/cg_project.git
cd cg_project
```

### Daily workflow for each member
```bash
git pull                          # get latest before you start
# ... edit your scene file ...
git add scenes/scene3.cpp         # add YOUR file only
git commit -m "Member 3: added rotation demo"
git push                          # send it up
```

### When merging
Because each person edits a different file, `git pull` just combines them automatically.
**No manual merging needed in 95% of cases.**

---

## Option B — No Git (Manual file sharing)

### How it works
- Each member edits only their `sceneX.cpp`
- When done, they send that one file to Member 1 (WhatsApp / Drive / email)
- Member 1 drops each file into the `scenes/` folder and runs `make`

### Why it can work here
We designed the project so each member touches exactly one file. So there's nothing
to "merge" — Member 1 just collects 4 files and drops them in.

### The risks
- If two people accidentally edit the same file, one version gets lost
- No history — if something breaks, you can't easily go back
- Lots of file versions floating around ("which one is latest?")
- Manual collection every time someone makes a change

---

## My Recommendation

| Your situation | Use |
|----------------|-----|
| Team is comfortable with terminal / has used Git | **Git + GitHub** |
| Team is new to Git and deadline is tight | **Git is still worth 30 min** |
| Absolutely no time to learn Git | Manual sharing is OK (we built for it) |

**Best middle ground:** Use GitHub Desktop (a visual app, no terminal needed). Everyone
installs it, clones the repo, and clicks "commit" + "push" with buttons. Gets you all
the safety of Git without memorizing commands.

---

## What to put in .gitignore

If you use Git, create a file called `.gitignore` so you don't commit compiled junk:

```
# Compiled output
cg_project
*.o
*.exe

# OS files
.DS_Store
Thumbs.db

# Editor files
.vscode/
*.swp
```

(This file is already included in your project folder.)

---

## Bottom line
The project is built to work **either way**. Git is the better choice and not as scary
as it sounds — but if your team genuinely can't manage it, the one-file-per-member
structure means manual sharing won't cause chaos.
