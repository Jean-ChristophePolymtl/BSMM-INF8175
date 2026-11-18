### 1. Charger les environnements

```bash
cd /export/tmp/allaire/bismo
source setup.csh

cd /export/tmp/allaire/bismo/mMIMO-ART-FPGA/fpga
source setup.csh
```

### 2. Sélectionner le Block Design QDMA

Dans `scripts/vivado_syn.tcl`, vérifier que `BD_TCL` pointe vers le Block Design QDMA:

```bash
set BD_TCL "$env(PROJECT_HOME)/block_designs/design_qdma.tcl"
```

### 3. Synthétiser

```bash
make syn ARCH=w4a4_qdma_test
```

### 4. Modifier et compiler le programme MicroBlaze

Mettre le contenu de `qdma.c` dans le fichier suivant :

```bash
cd /export/tmp/allaire/bismo/mMIMO-ART-FPGA/fpga/Vitis/src
nano main.c

cd /export/tmp/allaire/bismo/mMIMO-ART-FPGA/fpga
make build ARCH=w4a4_qdma_test
```

### 5. Programmer le FPGA et le MicroBlaze

```bash
make prog ARCH=w4a4_qdma_test
```

### 6. Initialiser le QDMA

Il faut attacher le driver `qdma_pf` au périphérique de la carte:

```bash
sudo /export/tmp/humblet/PolyMTL/u280_pcie_hbm/load-bitstream-onic.sh qdma /export/tmp/allaire/bismo/mMIMO-ART-FPGA/fpga/vivado/builds/w4a4_qdma_test/w4a4_qdma_test.runs/impl_1/design_1_wrapper.bit
```

La sortie doit contenir :

```text
Kernel driver in use: qdma-pf
```

### 7. Générer les données d'entrée du Transformer

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install numpy
python3 sripts/generate_demo_input.py microblaze/demo_test_vector.h
```

### 8. Lancer l'inférence

Depuis le dossier contenant `input.bin` :

```bash
chmod +x run_transformer_qdma.sh
./run_transformer_qdma.sh
```