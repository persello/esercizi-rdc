--- FILE E LIBRERIE ---

lib/              -> Contiene le librerie (sia quelle fornite dal docente, che altre, sviluppate per questi esercizi).
.../log           -> Libreria di logging con 3 livelli di dettaglio.
.../mastermind    -> Libreria per la risoluzione del gioco "Mastermind" secondo l'algoritmo di Knuth (adattato per funzionare in qualsiasi base e dimensione).
.../ringbuffer    -> Libreria per la creazione semplice di buffer circolari, usata per la ricezione di messaggi TCP. Utile per riassemblarli fino a formare una riga completa. Libreria recuperata da altri progetti personali in ambito embedded.
.../*socket       -> Librerie fornite dal docente, opportunamente modificate per funzionare con CMake. Aggiunti alcuni cast espliciti per evitare warning, senza modificarne il funzionamento.

output/parallel_check.sh      -> Script per effettuare 100 richieste di iscrizione alla gara podistica praticamente contemporanee.

--- COMPATIBILITÀ ---

I programmi sono stati testati su Linux Ubuntu 20.04.2.0 LTS (ARMv8/AArch64) con i seguenti compilatori:
  - GCC     9.3.0  aarch64-linux-gnu                            (1 warning, -Wall -Wpedantic -Wextra -Wno-unknown-pragmas -Wno-padded -Wno-unused-macros)

(Testato su una macchina virtuale appena creata, su cui sono stati installati solo Git e CMake (con le relative dipendenze) mediante il comando `sudo apt install git cmake`).

I programmi sono stati testati su macOS 11.4 con i seguenti compilatori:
  - Clang  12.0.5  arm64-apple-darwin20.5.0                     (nessun warning, -Weverything -Wno-documentation-unknown-command -Wno-padded -Wno-unused-macros)
  - Clang  12.0.5  x86_64-apple-darwin20.5.0  (Rosetta)         (nessun warning, -Weverything -Wno-documentation-unknown-command -Wno-padded -Wno-unused-macros)
  - GCC    11.1.0  aarch64-apple-darwin20                       (nessun warning, -Wall -Wpedantic -Wextra -Wno-unknown-pragmas -Wno-padded -Wno-unused-macros)

--- COMPILAZIONE ---

Compilare gli esercizi con CMake:

```
mkdir build && cd build
cmake ..
make
make install
```

Oppure usando lo script `build.sh`.

I binari risultanti verranno installati nella cartella Esercizi/output.

--- --- --- --- --- --- --- --- --- --- ---

Sorgente disponibile al link https://github.com/persello/esercizi-rdc.