# cocotb

This repository contains two Git submodules:

- `Coco_Questasim`: a counter and cocotb test for QuestaSim.
- `Python4RTLVerification`: Python and RTL verification examples.

Clone the repository and both submodules with:

```bash
git clone --recurse-submodules https://github.com/I-M-AJ/cocotb.git
```

For an existing clone, initialize the submodules with:

```bash
git submodule update --init --recursive
```

Each submodule is a separate repository. Commit and push its changes there,
then commit the updated submodule reference in this repository.
