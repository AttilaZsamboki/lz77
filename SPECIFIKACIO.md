# LZ77 Tömörítő Program - Részletes Specifikáció

## 1. A Program Célja és Feladatleírása

A program célja egy LZ77 algoritmus alapú veszteségmentes adattömörítő és kicsomagoló rendszer megvalósítása. A program képes tetszőleges fájlok tömörítésére és visszafejtésére, miközben garantálja, hogy a dekódolt fájl bájtonként megegyezik az eredetivel.

### 1.1 Főbb Funkciók

- **Tömörítés (encoding)**: Bemeneti fájl tömörítése LZ77 algoritmussal
- **Kicsomagolás (decoding)**: Tömörített fájl visszafejtése eredeti formájába

### 1.2 Az LZ77 Algoritmus Működése

Az algoritmus egy csúszóablak (sliding window) technikát használ:
- **Keresési puffer (search buffer)**: 4096 bájt méretű körpuffer, amely a már feldolgozott adatokat tárolja
- **Előretekintő puffer (lookahead buffer)**: Maximum 50 karakter, amely a még feldolgozandó adatokat tartalmazza
- A tömörítés során a program a keresési pufferben keresi a leghosszabb egyezést az előretekintő puffer tartalmával
- Minden egyezést egy token reprezentál: (távolság, hossz, következő karakter)

## 2. Bemenetek Specifikációja

### 2.1 Tömörítés (Encoder)

**Parancssori használat:**
```
encoder.exe <bemeneti_fájl> <kimeneti_fájl>
```

**Paraméterek:**
- `<bemeneti_fájl>`: Tetszőleges fájl teljes vagy relatív elérési útja
  - Típus: Bármilyen fájl (szöveges vagy bináris)
  - Méret: Korlátlan (a rendelkezésre álló memória függvényében)
  - Kódolás: Bármilyen (a program bináris módban dolgozik)

- `<kimeneti_fájl>`: A tömörített fájl elérési útja
  - Ajánlott kiterjesztés: `.lz77`
  - Ha a fájl létezik, felülíródik
  - Ha a könyvtár nem létezik, hibaüzenet jelenik meg

**Hibakezelés:**
- Ha nem pontosan 2 parancssori argumentum érkezik, a program kiírja a használati útmutatót és 1-es hibakóddal kilép
- Ha a bemeneti fájl nem nyitható meg, "File open failed!" hibaüzenet jelenik meg és a program 1-es hibakóddal kilép
- Ha a kimeneti fájl nem hozható létre, "File open failed!" hibaüzenet jelenik meg

### 2.2 Kicsomagolás (Decoder)

**Parancssori használat:**
```
decoder.exe <bemeneti_fájl> <kimeneti_fájl>
```

**Paraméterek:**
- `<bemeneti_fájl>`: Tömörített fájl elérési útja
  - Típus: LZ77 formátumú bináris fájl (`.lz77` kiterjesztés ajánlott)
  - Formátum: Token-ek sorozata (lásd 3.2 fejezet)

- `<kimeneti_fájl>`: A dekódolt fájl elérési útja
  - A fájl szöveges módban íródik ("w" mód)
  - Ha létezik, felülíródik

**Hibakezelés:**
- Ha nem pontosan 2 parancssori argumentum érkezik, a program kiírja a használati útmutatót és 1-es hibakóddal kilép
- Ha a bemeneti fájl nem nyitható meg, "Error opening file" hibaüzenet jelenik meg a standard error-on
- Ha memóriafoglalás sikertelen, "malloc" hibaüzenet jelenik meg


## 3. Kimenetek Specifikációja

### 3.1 Sikeres Futás

**Tömörítés esetén:**
- Kimeneti fájl: Bináris fájl, amely Token struktúrák sorozatát tartalmazza
- Visszatérési érték: 0
- Standard output: Nincs kimenet

**Kicsomagolás esetén:**
- Kimeneti fájl: Az eredeti fájl pontos másolata
- Visszatérési érték: 0
- Standard output: Egy üres sor (`\n`)

### 3.2 Token Formátum

Minden token 4 bájt méretű struktúra:

```c
typedef struct Token {
    uint16_t distance;  // 2 bájt: távolság (0-65535)
    uint8_t length;     // 1 bájt: hossz (0-255)
    unsigned char next; // 1 bájt: következő karakter
} Token;
```

**Token értelmezése:**
- Ha `distance == 0` és `length == 0`: Nincs egyezés, csak a `next` karaktert kell kiírni
- Egyébként: Másolni kell `length + 1` karaktert a `distance` távolságról, majd hozzáadni a `next` karaktert

**Példa:**
- Token: {distance=5, length=2, next='x'}
- Jelentés: Másold a 3 karaktert (length+1) az 5 pozícióval korábbról, majd add hozzá az 'x' karaktert

### 3.3 Tömörítési Arány

A tömörítés hatékonysága a bemeneti fájl jellegétől függ:
- **Ismétlődő mintázatokat tartalmazó fájlok**: Jelentős tömörítés (akár 50-90%)
- **Véletlenszerű adatok**: Minimális vagy negatív tömörítés (a fájl nagyobb lehet)
- **Szöveges fájlok**: Általában 30-60% tömörítés

## 4. A Program Használata

### 4.1 Fordítás

**Windows (GCC/MinGW):**
```bash
gcc -o encoder.exe encode.c circular_buffer.c
gcc -o decoder.exe decoder.c
```

**Linux/Unix:**
```bash
gcc -o encoder encode.c circular_buffer.c
gcc -o decoder decoder.c
```

### 4.2 Használati Példák

**Fájl tömörítése parancssori módban:**
```bash
encoder.exe document.txt document.lz77
```

**Fájl kicsomagolása parancssori módban:**
```bash
decoder.exe document.lz77 document_restored.txt
```

**Tömörítés és kicsomagolás ellenőrzése:**
```bash
# Tömörítés
encoder.exe original.txt compressed.lz77

# Kicsomagolás
decoder.exe compressed.lz77 restored.txt

# Ellenőrzés (Windows PowerShell)
fc /b original.txt restored.txt

# Ellenőrzés (Linux)
diff original.txt restored.txt
```

### 4.3 Tipikus Munkafolyamat

1. **Tömörítés előtt**: Ellenőrizd, hogy elegendő szabad lemezterület áll-e rendelkezésre
2. **Tömörítés**: Futtasd az encoder-t a bemeneti fájlon
3. **Ellenőrzés**: Futtasd a decoder-t és hasonlítsd össze az eredeti fájllal
4. **Archiválás**: Tárold a `.lz77` fájlt, az eredeti törölhető (biztonsági mentés után)

## 5. Korlátozások és Megkötések

### 5.1 Technikai Korlátozások

- **Keresési puffer mérete**: Fix 4096 bájt (LENGTH konstans)
- **Előretekintő puffer**: Maximum 50 karakter
- **Maximális egyezési távolság**: 65535 bájt (uint16_t limit)
- **Maximális egyezési hossz**: 256 karakter (uint8_t limit, 0-255 + 1)
- **Memóriahasználat**: A bemeneti fájl teljes egészében a memóriába kerül betöltésre

### 5.2 Fájlméret Korlátozások

- **Bemeneti fájl**: A rendelkezésre álló RAM mérete korlátozza
- **Kimeneti fájl**: A lemezterület korlátozza
- **Gyakorlati ajánlás**: Maximum néhány száz MB méretű fájlok esetén optimális

### 5.3 Fájltípus Korlátozások

- **Támogatott**: Minden fájltípus (szöveges, bináris, képek, dokumentumok)
- **Nem támogatott**: Könyvtárak, speciális fájlok (device files, pipes)
- **Megjegyzés**: Már tömörített fájlok (ZIP, GZIP, JPEG) esetén a tömörítés hatástalan vagy kontraproduktív lehet

### 5.4 Platform Korlátozások

- **Elsődleges platform**: Windows
- **Kompatibilitás**: A C kód platformfüggetlen
- **Portolhatóság**: Linux/Unix rendszerekre portolható

## 6. Hibakezelés és Hibaüzenetek

### 6.1 Encoder Hibaüzenetek

| Hibaüzenet | Ok | Megoldás |
|------------|-----|----------|
| "Usage: ..." | Helytelen számú parancssori argumentum | Add meg pontosan 2 argumentumot |
| "File open failed!" | Bemeneti fájl nem található vagy nem olvasható | Ellenőrizd a fájl elérési útját és jogosultságokat |
| "File open failed!" | Kimeneti fájl nem hozható létre | Ellenőrizd a könyvtár létezését és írási jogokat |

### 6.2 Decoder Hibaüzenetek

| Hibaüzenet | Ok | Megoldás |
|------------|-----|----------|
| "Usage: ..." | Helytelen számú parancssori argumentum | Add meg pontosan 2 argumentumot |
| "Error opening file" | Bemeneti fájl nem található | Ellenőrizd a fájl elérési útját |
| "malloc" | Nincs elég memória | Zárd be a többi programot vagy használj kisebb fájlt |

### 6.3 Várt Viselkedés Hibás Bemenet Esetén

- **Nem létező fájl**: Hibaüzenet és azonnali kilépés
- **Sérült LZ77 fájl**: A decoder megpróbálja feldolgozni, de az eredmény hibás lehet
- **Jogosultsági problémák**: Rendszer szintű hibaüzenet (permission denied)

## 7. Teljesítmény és Hatékonyság

### 7.1 Várható Futási Idő

- **Kis fájlok (< 1 MB)**: < 1 másodperc
- **Közepes fájlok (1-10 MB)**: 1-10 másodperc
- **Nagy fájlok (> 10 MB)**: Lineárisan növekszik a fájlmérettel

### 7.2 Memóriahasználat

- **Encoder**: ~(fájlméret + 4096 * 8 bájt) memória
- **Decoder**: ~(fájlméret * 10) memória (biztonsági tartalék a dekompresszióhoz)

### 7.3 Optimalizációs Lehetőségek

Jelen specifikáció szerint a program nem tartalmaz speciális optimalizációkat. A működés helyességére fókuszál, nem a sebességre.

## 8. Tesztelési Követelmények

### 8.1 Funkcionális Tesztek

1. **Üres fájl**: Az encoder és decoder helyesen kezelje az üres bemenetet
2. **Egykarakteres fájl**: Helyes tömörítés és visszafejtés
3. **Ismétlődő karakterek**: "AAAAAAAAAA" típusú fájlok hatékony tömörítése
4. **Véletlenszerű adatok**: Működjön, még ha nem is tömörít
5. **Bináris fájlok**: Képek, exe fájlok helyes kezelése
6. **Nagy fájlok**: Több MB méretű fájlok tesztelése

### 8.2 Ellenőrzési Módszer

Minden tesztnél:
```bash
encoder.exe test_input.txt test.lz77
decoder.exe test.lz77 test_output.txt
# Az eredeti és a dekódolt fájl bájtonként megegyezik
```

### 8.3 Sikerkritérium

A program akkor tekinthető helyesnek, ha:
- Minden tesztfájl esetén: `original == decoded` (bájtonként azonos)
- Ismétlődő mintázatok esetén mérhető tömörítés történik
- Nem lép fel memóriaszivárgás vagy crash

## 10. Függelékek

### 10.1 Fájlstruktúra

```
lz77/
├── circular_buffer.h       # Körpuffer és egyezéskereső függvények deklarációi
├── circular_buffer.c       # Körpuffer implementáció
├── encode.c                # Tömörítő program
├── decoder.c               # Kicsomagoló program
├── encoder.exe            # Fordított tömörítő (Windows)
├── decoder.exe            # Fordított kicsomagoló (Windows)
└── README.md              # Projekt dokumentáció
```

### 10.2 Adatstruktúrák

**Match struktúra** (circular_buffer.h):
```c
typedef struct Match {
    int d;      // distance: távolság a keresési pufferben
    int l;      // length: egyezés hossza
    char c;     // character: következő karakter
} Match;
```

**Token struktúra** (encode.c, decoder.c):
```c
typedef struct Token {
    uint16_t distance;      // 2 bájt
    uint8_t length;         // 1 bájt
    unsigned char next;     // 1 bájt
} Token;
```

### 10.3 Konstansok

- `LENGTH`: 4096 - A keresési puffer (circular buffer) mérete
- Előretekintő puffer: 50 karakter (hardcoded a `find_match` függvényben)

**Verzió**: 1.0  
**Utolsó módosítás**: 2025. október 29.  
**Státusz**: Végleges specifikáció
