
# Tiedostohierarkia

```
project-root/
├── Makefile
├── build/                  # Käännetyt tiedostot (objektit, binäärit jne.)
├── iso/                    # GRUB:in ISO-tiedostot
│   └── grub/               # GRUB:n konfiguraatio
├── src/
│   ├── boot/               # Bootloader-koodi, esim. assembly-tiedostot
│   ├── kernel/             # Kernelin pääkoodi ja korkeampi taso
│   ├── drivers/            # Ajurit (esim. portit, sarjaportti jne.)
│   ├── lib/                # Yhteiset kirjastot ja apufunktiot
│   └── arch/               # Arkkitehtuuri- ja alustariippuvaiset koodit
│       └── x86_64/         # x86_64-arkkitehtuuriin liittyvät asiat
│           ├── gdt/        # GDT:hen liittyvä koodi
│           ├── idt/        # IDT ja keskeytyksiin liittyvä koodi
│           └── paging/     # Sivutukseen liittyvä koodi
└── docs/                   # Dokumentaatio, kuten asennus- ja käyttöohjeet
```

## src/boot/:
Bootloader-koodi ja kaikki assembly-tiedostot, jotka liittyvät käynnistysprosessiin, kuten GRUB
liittyvät tiedot.

## src/kernel/:
Kernelin keskeinen C-koodi, kuten pääfunktiot ja korkeamman tason logiikka.
Pääosin koodi, joka käyttää tai hallitsee ajureita ja muistinhallintaa.

## src/drivers/:
Sarjaportti, I/O-porttien hallinta, näppäimistö, hiiri jne. Voit luoda esimerkiksi tiedoston serial.c tai ports.c, jotka sisältävät sarjaporttiin ja muihin portteihin liittyvät toiminnot.
Tämä tekee ajureiden hallinnasta selkeää ja erottaa laitteistokohtaiset asiat kernelin ydinlogiikasta.

## src/lib/:
Yleiset apufunktiot, kuten matemaattiset operaatiot, bittien käsittely tai muistinhallinnan apufunktiot, joita tarvitaan useissa paikoissa.

## src/arch/x86_64/:
Kaikki arkkitehtuuriin sidottu koodi, kuten GDT, IDT, ja sivutus (paging).
Tämä mahdollistaa projektin tulevaisuudessa helpomman laajennettavuuden muille arkkitehtuureille

## docs/:
Dokkumentaatio kuten README.md tai koodin käyttöönottoa ja laajentamista koskevia muistiinpanoja.
