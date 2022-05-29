# PongGame-v2.0 

Joc de Pong 1v1 cu obstacole si elemente de pick-up care apar in timpul jocului. 
Jocul este implementat pe o placuta Arduino Uno, afisarea jocului facandu-se pe un OLED Display 0.96” 128×64 SPI. 
Controlul jucatorilor se realizeaza prin intermediul butoanelor, existand si un buton special pentru schimbarea directiei bilei. 
Este folosit un buzzer pentru a semnala coliziunile bilei cu terenul si obstacolele.  
  
 ### Implementare 
 ------------------ 
 - Inainte de de incepera jocului este afisat un meniu ce contiune optiunile jocului. Optiunile reprezinta punctajul pana la care se poate juca, acestea fiind: 3, 5 sau 7 puncte. 
 - Navigarea prin meniu si alegerea optiunii se realizeaza prin intermediul butoanelor celui de al doilea jucator.  
 - Dupa ce optiunea este selectata, jucatorii se pot pregati de joc in timp ce bara de loading se incarca pana la 100%.
 - Jucatorii sunt reprezentati de cate o bara verticala, mingea de un pixel, obstacolele de patrulatere de diferite dimensiuni, iar elementul de pick-up de un cerc.
 - Jucatorii se deplaseaza doar pe axa OY, iar mingea pe XOY.
 - Pe parcursul jocului se afiseaza scorul curent si cate elemente de pick-up au colectat jucatorii.
 - Atunci cand un jucator ia un element de pick-up, atunci el are posibilitatea de a schimba directia de mers a bilei atunci cand vrea.  
 
 #### Desfasurarea jocului
 ------------------------ 
 - la inceputul fiecarei runde se alege random in ce directie porneste bila 
 - jucatorii trebuie sa isi apere peretele din spatele fiecaruia, astfel daca mingea o liveste se pune punct pentru adversar 
 - pe parcursul jocului apar obstacole si elemente de pick-up, pe pozitii random 
 - elementul de pick-up apare pe teren dupa o anumita perioada de timp dupa colectarea acestuia
 - obstacolul isi schimba pozitia si dimensiunea dupa ce este lovit de minge sau la inceperea unei noi runde
 - jocul se termina atunci cand unul dintre jucatori ajunge la numarul maxim de puncte
 - la sfarsit se afiseaza pe display castigatorul si se sarbatoresete castigul lui prin redearea unei piese la buzzer
 
 
 ### Resurse
 -----------
 - https://www.youtube.com/watch?v=u5drIsmr6UM&ab_channel=%D0%9A%D0%BE%D0%BD%D1%81%D1%82%D0%B0%D0%BD%D1%82%D0%B8%D0%BD
 - https://create.arduino.cc/projecthub/ronfrtek/arduino-oled-display-menu-with-option-to-select-e85f04
 - https://create.arduino.cc/projecthub/GeneralSpud/passive-buzzer-song-take-on-me-by-a-ha-0f04a8
 - https://gist.github.com/mikeputnam/2820675
