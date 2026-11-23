demo2d : précodeur MU-MIMO (réseau 4x4, 4 utilisateurs, Alveo U280)

Le bloc reçoit le canal H (4 utilisateurs x 16 antennes) et rend W (16 x 4).
À appliquer sur les antennes : conj(W). Avec W tel quel, le débit est presque nul.

Calcul à faire (H : 4 utilisateurs x 16 antennes, H[k,n] = canal antenne n -> utilisateur k)
  1. A = conj(H) / ||H||               (norme sur les 64 coefficients)
  2. x[k*32 + 2n] = Im A[k,n],  x[k*32 + 2n + 1] = Re A[k,n]
     envoyer int32 = floor(x * 2^22)   (128 mots)
  3. lire y (128 float). Pour l'utilisateur k : w_k[n] = y[k*32+n] + j*y[k*32+16+n]
  4. W[n,k] = w_k[n] / ||W||           (W : 16 x 4, norme sur les 64 coefficients)
  5. précodeur à appliquer : W_tx = conj(W). Reçu = H @ W_tx.

Lancer un calcul (MicroBlaze, registres AXI-Lite)
  1. Écrire l'entrée en mémoire, flush du cache.
  2. Adresse entrée : 0x10 (bas), 0x14 (haut). Sortie : 0x1C, 0x20.
  3. Attendre que le bloc soit libre (bit 2 de 0x00 = 1), écrire 1 à 0x00
     (start), puis attendre bit 1 de 0x00 = 1 (done).
  4. Invalider le cache de la sortie, lire les 128 float.
Le pilote C est dans l'IP (drivers/).
Horloge 100 MHz, durée ~16 µs + accès mémoire.

Vérification : exemples.csv (example, quantity, row, col, re, im)
  quantity = H, x (entrée), y (sortie attendue), W, W_tx (= conj(W))
  1. Ton code doit produire x à partir de H.
  2. Envoie x au FPGA : la sortie doit égaler y (écart ~1e-6).
