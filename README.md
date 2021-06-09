# Esercizi di Reti di Calcolatori

| Esercitazione | Relazione        | Allegati                             |
| ------------- | ---------------- | ------------------------------------ |
| NA01          | [PDF](NA01.pdf)  |                                      |
| NA02          | [PDF](NA02.pdf)  |                                      |
| NA03          | [PDF](NA03.pdf)  |                                      |
| NSD01         | [PDF](NSD01.pdf) | [Cartella esercizi](Esercizi/NSD01/) |

---

Compilare i file PDF con `make`.

Compilare gli esercizi con CMake:

```sh
cd Esercizi
mkdir build && cd build
cmake ..
make
make install
```

I binari risultanti verranno installati nella cartella `Esercizi/output`, suddivisi per esercizio.