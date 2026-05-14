# Transkript

## Metadaten
- Quelle: VTT
- Datum: 2026-04-02
- Datei: Zk_2026-04-02.vtt

## Segmente

| Start | Ende | Sprecher | Text |
|---|---|---|---|
| 00:00:01,639 | 00:00:03,080 | Robert | Ne, aber dann legen wir los, oder? |
| 00:00:03,260 | 00:00:04,600 | Marcel | Genau. Ich würde sagen, wir definieren... |
| 00:00:04,600 | 00:00:05,120 | Marcel | Ich sage es auch? |
| 00:00:05,600 | 00:00:10,880 | Marcel | Genau, ich würde sagen, wir haben jetzt die Aufzeichnung gerade nochmal gestoppt und neu gestartet, damit wir den Summs nicht mit dabei haben. |
| 00:00:12,400 | 00:00:16,180 | Marcel | Ich würde vorschlagen, wir definieren heute Use Cases. |
| 00:00:17,859 | 00:00:24,240 | Marcel | Und wenn ich rede und sage "ich", dann bezieht sich das auf die Rolle der Zugangskontrolle, ZK. |
| 00:00:25,200 | 00:00:28,100 | Marcel | Und was die macht, also das Gerät, was im Auto verbaut ist, auf dem Arduino. |
| 00:00:28,640 | 00:00:36,980 | Marcel | und wenn du von dir redest oder auch vom Sharepad dann ist das quasi immer das Backend von dieser Lösung, was das liefert, was das abliefert. |
| 00:00:37,790 | 00:00:40,140 | Marcel | Macht, tut, wie es reagiert. Genau. |
| 00:00:40,440 | 00:00:53,720 | Robert | Bei den Nutzer müssen wir unterscheiden zwischen Admin und ich würde sagen, also wir unterscheiden zwischen Nutzer und Admin. |
| 00:00:54,720 | 00:00:57,920 | Robert | Also ein Admin ist derjenige, der das System administriert, ist klar. |
| 00:00:59,520 | 00:01:05,480 | Robert | Nutzer ist der, der fährt und Mitglied ist eine Gruppe von Nutzern, |
| 00:01:06,830 | 00:01:12,700 | Robert | die unter der Mitgliedsnummer zusammengefasst ist und die abrechnungsrelevant ist. |
| 00:01:13,320 | 00:01:18,480 | Marcel | Genau, also ein Mitglied ist ein Verbund von Nutzern, die sich eine Abrechnungskonto teilen. |
| 00:01:20,600 | 00:01:31,760 | Marcel | Ein Mitglied gehört immer zu einem Verein, wobei der Verein, haben wir festgelegt, in unserer Konstellation wahrscheinlich relativ wenig relevant sein wird. |
| 00:01:31,760 | 00:01:35,080 | Marcel | Der ist für dich intern im Backend natürlich relevant, aber für die ZK weniger. |
| 00:01:36,160 | 00:01:44,740 | Marcel | Das Mitglied ist der Inhaber des Führerscheins, der einen Führerschein mit einem NFC-Aufkleber beklebt hat und darauf eine UID hat. |
| 00:01:46,780 | 00:01:53,200 | Marcel | Entsprechend hat ein Mitglied 1 bis N UIDs. |
| 00:01:54,420 | 00:01:56,800 | Marcel | Und jetzt gibt es verschiedene Szenarien. |
| 00:01:58,240 | 00:02:02,140 | Marcel | Wenn man das chronologisch der Reihe nach durchgeht, wäre der erste Use Case, |
| 00:02:03,380 | 00:02:05,020 | Marcel | ich buche mir ein Auto. |
| 00:02:05,500 | 00:02:07,000 | Marcel | Der hat nur mit dem Backend zu tun. |
| 00:02:07,540 | 00:02:09,660 | Marcel | Da wird im Endeffekt ein Eintrag erzeugt. |
| 00:02:10,380 | 00:02:13,160 | Marcel | Ich melde mich als Nutzer an. |
| 00:02:14,860 | 00:02:27,820 | Marcel | Darüber bin ich einem Mitglied zugeordnet, beziehungsweise kann eventuell, das musst du in einer Software regeln, mehrere Mitglieder, bei denen mein Nutzer Zugriff hat, auswählen. |
| 00:02:27,860 | 00:02:31,200 | Marcel | Ein Beispiel war ja der User Marcel Meier. |
| 00:02:32,340 | 00:02:39,580 | Marcel | Der ist sowohl Privatmitglied als Familie Meier, als auch Mitglied bei der Firma Dondenville, |
| 00:02:40,500 | 00:02:46,660 | Marcel | die das als gewerbliche Mitglied sozusagen auch beim Autoteilerverein Mitglied ist. |
| 00:02:47,360 | 00:02:53,580 | Marcel | Und somit habe ich mit einem Nutzer zwei Mitgliedsnummern und kann dann auswählen bei der Buchung, |
| 00:02:53,620 | 00:02:55,380 | Marcel | mit welcher Mitgliedsnummer mache ich die Buchung. |
| 00:02:56,280 | 00:03:03,060 | Marcel | Und das ist dann in späteren Instanz definitiv schon relevant, weil du speicherst dann ab. |
| 00:03:03,720 | 00:03:18,300 | Marcel | Der Benutzer Marcel Maier hat mit dem Mitglied Dondenville von Uhr bis Uhr, von Datum bis Datum, folgende Ressource, also sprich folgendes Auto, reserviert. |
| 00:03:19,540 | 00:03:19,900 | Marcel | Genau. |
| 00:03:20,740 | 00:03:21,000 | Marcel | Genau. |
| 00:03:22,260 | 00:03:25,020 | Marcel | Dann pflegt es, bestätigt. |
| 00:03:25,540 | 00:03:29,180 | Marcel | Damit ist die Buchung aus User-Sicht erstmal abgeschlossen. |
| 00:03:31,160 | 00:03:34,480 | Marcel | Dann kommt Tag X, wo ich sage, jetzt möchte ich das Auto fahren. |
| 00:03:35,780 | 00:03:37,900 | Marcel | Meine Reservierung ist gemacht. |
| 00:03:38,320 | 00:03:42,880 | Marcel | Ich gehe zu dem Auto hin und möchte Zugang zu dem Auto erlangen. |
| 00:03:43,040 | 00:03:44,320 | Marcel | Also möchte das Auto aufsteigen können. |
| 00:03:46,500 | 00:03:47,440 | Marcel | Wäre der Use Case 2. |
| 00:03:48,700 | 00:03:49,340 | Marcel | Auto öffnen. |
| 00:03:51,140 | 00:03:51,720 | Marcel | Dann gehe ich hin. |
| 00:03:53,680 | 00:03:54,800 | Marcel | Leg meine Karte drauf. |
| 00:03:55,940 | 00:04:05,060 | Marcel | auf den NFC Reader, der liest meine UID, später vergleicht er noch, ob die UID die gleiche UID ist wie im verschlüsselten Sektor 1 |
| 00:04:05,260 | 00:04:07,580 | Marcel | und ob er den mit unserem Schlüssel entschlüsseln kann. |
| 00:04:08,920 | 00:04:17,140 | Marcel | Wenn ja, ist schon mal gut, dann prüft er, habe ich denn schon eine Reservierung für dieses Auto in meinem lokalen Speicher, |
| 00:04:17,220 | 00:04:18,540 | Marcel | bei mir und meiner in der ZK. |
| 00:04:19,720 | 00:04:22,760 | Marcel | Und da sind wir schon bei dem ersten technischen Schnittstelle, die wir brauchen, |
| 00:04:23,760 | 00:04:30,520 | Marcel | Weil das Auto muss entweder anfragen oder... |
| 00:04:31,540 | 00:04:35,440 | Marcel | Also entweder pusht du an das Auto, aber ich halte es für sinnvoller wahrscheinlich, dass ich pulle als Auto. |
| 00:04:36,480 | 00:04:37,660 | Marcel | Als ZK im Auto. |
| 00:04:38,900 | 00:04:42,940 | Marcel | Ich wüsste nicht, wie technisch Push umzusetzen wäre. |
| 00:04:43,620 | 00:04:52,780 | Marcel | Komplizierte, aber man könnte es rückauflösen, weil wir wissen, welche IP die ZK hat. |
| 00:04:52,960 | 00:04:54,860 | Marcel | und damit könntest du auch einen Push dorthin machen. |
| 00:04:55,500 | 00:04:57,000 | Marcel | Aber ich halte den Pull für viel sinnvoller. |
| 00:04:59,080 | 00:05:00,360 | Marcel | Später können wir immer noch mal überlegen, |
| 00:05:00,480 | 00:05:02,160 | Marcel | ob es unpraktisch ist, |
| 00:05:02,300 | 00:05:03,640 | Marcel | dann könnte man immer noch mal per |
| 00:05:05,240 | 00:05:06,740 | Marcel | MQTT oder irgendwas was machen, |
| 00:05:07,100 | 00:05:08,740 | Marcel | was praktikabel ist, |
| 00:05:09,140 | 00:05:10,400 | Marcel | dass man da irgendwelche Channels hat, |
| 00:05:10,400 | 00:05:11,760 | Marcel | die man dann abonnieren muss als Auto |
| 00:05:11,900 | 00:05:13,580 | Marcel | und dann kriege ich die Infos mit oder umgekehrt. |
| 00:05:14,060 | 00:05:14,960 | Marcel | Halte ich aber jetzt für einen Quatsch. |
| 00:05:16,060 | 00:05:18,220 | Robert | Ein Push ist ja erst einmal technisch |
| 00:05:19,900 | 00:05:21,140 | Robert | geht der viel tiefer, |
| 00:05:21,280 | 00:05:22,920 | Robert | weil da musst du |
| 00:05:23,000 | 00:05:33,640 | Robert | von Serverseite aus die Verbindung, also der Chip im Auto, der Funkchip, der geht ja in den Schlafmodus. |
| 00:05:34,260 | 00:05:37,400 | Robert | Und der muss aber von Serverseite aus weckbar sein. |
| 00:05:38,180 | 00:05:45,900 | Robert | Und das ist natürlich eine ganz andere Liga, wie wenn ich regelmäßig oder bei Bedarf anrufe und einen Pull mache. |
| 00:05:46,160 | 00:05:48,500 | Marcel | Genau, deswegen bleiben wir absolut beim Pull. |
| 00:05:49,280 | 00:05:52,440 | Marcel | Stimmt, und das Schlafmodus von der Hardware habe ich gar nicht gedacht. |
| 00:05:53,400 | 00:06:04,380 | Marcel | Die ist ja nochmal verkompliziert das ganze noch mehr. Also vergiss was ich gesagt habe, es gibt an Absolut die ZK Pult in regelmäßigen Abständen von Dir ihre Termine. |
| 00:06:04,380 | 00:06:13,460 | Marcel | Das heißt, sie muss sagen: Lieber Robert, Liebes Sherpard, ich bin das Auto mit der ID XYZ. |
| 00:06:16,800 | 00:06:19,300 | Marcel | Oder mit dem Namen oder irgendwas anderem. |
| 00:06:22,280 | 00:06:23,840 | Marcel | Mit welcher ID? |
| 00:06:24,900 | 00:06:28,340 | Marcel | Wahrscheinlich am ehesten die E-Mail? |
| 00:06:28,340 | 00:06:32,680 | Robert | Also ich intern arbeite ausschließlich mit Dual IDs. |
| 00:06:35,660 | 00:06:48,280 | Robert | Beim Auto haben wir jetzt gerade bei den Nutzeridentifikationen, da haben wir jetzt auch die Nummer der Karte, |
| 00:06:48,470 | 00:06:55,940 | Robert | also die ULE der MyFair-Karte genommen, was auch Sinn macht, weil da haben wir viele davon und da müssen wir |
| 00:06:55,980 | 00:06:57,100 | Robert | Platz sparen eventuell. |
| 00:06:58,460 | 00:07:00,040 | Robert | Ich könnte mir vorstellen, dass man |
| 00:07:00,460 | 00:07:00,580 | Robert | auf, |
| 00:07:02,580 | 00:07:04,240 | Robert | also um sich selbst zu identifizieren, |
| 00:07:04,680 | 00:07:06,200 | Robert | dass wir da auch mit der UID |
| 00:07:06,460 | 00:07:07,860 | Robert | arbeiten. Aber lassen wir das |
| 00:07:07,860 | 00:07:08,780 | Robert | mal außen vor. |
| 00:07:10,340 | 00:07:11,940 | Marcel | Das ist eine Sache, die kann man noch definieren, |
| 00:07:11,990 | 00:07:13,460 | Marcel | aber wir müssen im Endeffekt definieren, |
| 00:07:15,180 | 00:07:15,900 | Marcel | wie sagt |
| 00:07:16,060 | 00:07:17,840 | Marcel | das Auto, also die |
| 00:07:17,940 | 00:07:19,740 | Marcel | ZK, dem Backend, im Sharepad, |
| 00:07:20,020 | 00:07:21,480 | Marcel | was es für ein Auto ist, wenn es anfragt. |
| 00:07:21,510 | 00:07:23,640 | Marcel | Weil es soll ja nur die Termine von sich selbst, also |
| 00:07:23,710 | 00:07:25,640 | Marcel | von dem Auto kriegen. |
| 00:07:26,420 | 00:07:28,220 | Marcel | welche Termine, welche Reservierungen da vorliegen. |
| 00:07:28,860 | 00:07:30,300 | Robert | Also lass uns mal von einer ID |
| 00:07:30,500 | 00:07:31,260 | Robert | reden, woher die kommt. |
| 00:07:32,020 | 00:07:33,260 | Marcel | Also das Auto kriegt eine ID |
| 00:07:34,240 | 00:07:36,040 | Marcel | und mit dieser ID fragt es dich an und sagt |
| 00:07:36,140 | 00:07:37,820 | Marcel | ich bin Auto mit ID XYZ. |
| 00:07:38,780 | 00:07:40,300 | Marcel | Diese ID ist vereinsübergreifend |
| 00:07:40,460 | 00:07:42,140 | Marcel | eindeutig, der Einfachkeit halber würde ich |
| 00:07:42,240 | 00:07:44,200 | Marcel | sagen und damit sagt es einfach |
| 00:07:44,320 | 00:07:45,320 | Marcel | nur ich bin Auto mit der ID |
| 00:07:46,420 | 00:07:47,440 | Marcel | und dann sagst du, ah passt, |
| 00:07:48,060 | 00:07:50,080 | Marcel | suchst die Daten zusammen und schickst ihm |
| 00:07:50,240 | 00:07:51,720 | Marcel | die bekannten Reservierungen |
| 00:07:54,820 | 00:07:55,700 | Marcel | plus die |
| 00:07:55,720 | 00:07:57,440 | Marcel | Reservierungen, also welche User |
| 00:07:58,120 | 00:07:59,760 | Marcel | stehen hinter dem Mitglied, |
| 00:07:59,760 | 00:08:01,420 | Marcel | das gebucht hat, welche IDs |
| 00:08:01,760 | 00:08:03,360 | Marcel | haben die in ihren NFC-Karten. |
| 00:08:04,040 | 00:08:05,700 | Marcel | Diese Whitelist für die Reservierung |
| 00:08:05,860 | 00:08:07,800 | Marcel | schickst du mit und dann mit |
| 00:08:08,020 | 00:08:08,940 | Marcel | In den Reservierungen. |
| 00:08:09,800 | 00:08:10,960 | Marcel | Was? Mit den Reservierungen? |
| 00:08:11,200 | 00:08:13,280 | Robert | In den Reservierungen. Also in jeder Reservierung |
| 00:08:13,350 | 00:08:15,380 | Robert | ist eine reservierungsspezifische Whitelist. |
| 00:08:15,780 | 00:08:16,460 | Robert | Genau, richtig. |
| 00:08:17,020 | 00:08:19,680 | Robert | Plus, was ja auch mitgeschickt wird, |
| 00:08:19,750 | 00:08:20,980 | Robert | ist eine globale |
| 00:08:21,960 | 00:08:22,400 | Marcel | Whitelist. |
| 00:08:24,040 | 00:08:25,140 | Marcel | Ich würde das trennen. |
| 00:08:25,560 | 00:08:31,000 | Marcel | Das würde ich trennen. Aus meiner Sicht sind das separate Prozesse, die würde ich nicht bei jeder Abfrage mitschicken. |
| 00:08:31,460 | 00:08:35,680 | Marcel | Weil Reservierungsabfragen machst du ja nicht gut. |
| 00:08:35,800 | 00:08:40,979 | Marcel | Das ist die Frage, man kann sagen, ich würde es trennen, ob man es dann zeitgleich macht oder sequentiell hintereinander. |
| 00:08:41,000 | 00:08:42,580 | Marcel | Ich würde es aber in mehrere Prozesse trennen. |
| 00:08:43,960 | 00:08:44,340 | Marcel | Ja, okay. |
| 00:08:45,440 | 00:08:46,800 | Marcel | Dass man da eine schöne Trennung hat. |
| 00:08:46,840 | 00:08:50,960 | Marcel | Also sprich, der Prozess jetzt heißt, ich habe ja eigentlich schon, |
| 00:08:51,680 | 00:08:54,300 | Marcel | also wir waren beim Prozess Autoaufsperren, sind gerade abgeschweift, |
| 00:08:55,040 | 00:09:00,240 | Marcel | Er prüft, habe ich eine Reservierung? Wenn ja, steht die Karte in der Whitelist der Reservierung. |
| 00:09:00,860 | 00:09:03,180 | Marcel | Wenn nein, dann wird die Karte abgelehnt. |
| 00:09:07,340 | 00:09:10,640 | Marcel | Wenn die in der Whitelist steht, wird aufgesperrt. |
| 00:09:10,640 | 00:09:12,480 | Marcel | Wenn steht sie nicht in der Whitelist, wird abgelehnt. |
| 00:09:13,760 | 00:09:15,100 | Marcel | Gibt es gar keine Reservierung. |
| 00:09:16,120 | 00:09:22,080 | Marcel | Dann kommen wir auf technisch gesehen eine zweite Schnittstelle im Bereich Reservierungen. |
| 00:09:23,280 | 00:09:28,140 | Marcel | Nämlich die Einzelabfrage zum Thema, gibt es jetzt neue Reservierungen? |
| 00:09:28,280 | 00:09:31,020 | Marcel | Das ist die Frage, ob man die trennt, ob man da zwei daraus macht. |
| 00:09:31,700 | 00:09:37,020 | Marcel | Also sprich, einmal gibt man die Reservierung der nächsten 10 Tage, 14 Tage |
| 00:09:37,200 | 00:09:40,040 | Marcel | und einmal in der Abfrage gibt man die Reservierung von jetzt, |
| 00:09:42,560 | 00:09:45,880 | Marcel | sprich Vergangenheit plus eine Stunde oder sowas. |
| 00:09:46,690 | 00:09:49,720 | Marcel | Ob man das einschränkt? Ich glaube, wegen der Datenmenge macht das wenig Sinn. |
| 00:09:49,920 | 00:09:51,340 | Marcel | Ich würde das fest auf eine Größe machen. |
| 00:09:52,240 | 00:09:56,160 | Marcel | Es gibt eine Abfrage, die wird einmal zyklisch jeden Tag gemacht. |
| 00:09:57,260 | 00:10:05,020 | Marcel | Wenn die ZK sich Whitelist aktualisiert, Firmware prüft, Protokolle hochlädt, lädt sie sich auch die neuen Termine runter. |
| 00:10:05,220 | 00:10:14,880 | Marcel | Und dann gibt es den Sonderfall, dass wenn ich das Auto aufsperren möchte, ich stehe davor, lege die Karte auf, er sagt, ich finde aber jetzt gerade gar keinen Termin. |
| 00:10:16,480 | 00:10:19,720 | Marcel | Dann fragt er nochmal den Server an und sagt, gibt es denn vielleicht gerade was Neues? |
| 00:10:19,820 | 00:10:24,480 | Marcel | weil es kann ja sein, dass ich dort 10 Minuten erst die Buchung gemacht habe. |
| 00:10:25,740 | 00:10:28,520 | Marcel | Er lädt aber einmal danach die Liste runter, dann fragt er jetzt bei dir an |
| 00:10:29,240 | 00:10:32,440 | Marcel | und lädt die neue Reservierungsliste runter und stellt dann fest, |
| 00:10:32,640 | 00:10:34,760 | Marcel | ah, jetzt gibt es einen Termin, nachdem ich runtergeladen habe. |
| 00:10:35,560 | 00:10:38,860 | Marcel | Passt, meine Karte steht in der Whitelist, ich darf das Auto aufsperren. |
| 00:10:39,860 | 00:10:42,000 | Marcel | Während er runterlädt, da hat es ja ein bisschen dauert, |
| 00:10:42,260 | 00:10:47,560 | Marcel | weil du musst die Daten aufbereiten, du musst sie mir wieder zurückschicken, |
| 00:10:47,800 | 00:10:49,660 | Marcel | ich muss sie einlesen, muss die Reservierungen irgendwo |
| 00:10:49,660 | 00:10:51,880 | Marcel | in der ZK strukturiert mit Whitelist ablegen |
| 00:10:53,580 | 00:10:56,280 | Marcel | und dann nochmal meinen Terminkalender |
| 00:10:56,440 | 00:10:57,740 | Marcel | sozusagen durchforsten |
| 00:10:58,100 | 00:10:59,320 | Marcel | und die Reservierung finden. |
| 00:11:01,440 | 00:11:02,580 | Marcel | Und während er |
| 00:11:03,660 | 00:11:05,680 | Marcel | diesen Download macht und die Abfrage des |
| 00:11:05,680 | 00:11:07,960 | Marcel | ganzen, müsste man an der ZK |
| 00:11:08,100 | 00:11:09,620 | Marcel | irgendeine Visualisierung schaffen |
| 00:11:09,920 | 00:11:11,660 | Marcel | an der Windschutzscheibe, dass man sieht, |
| 00:11:11,700 | 00:11:12,340 | Marcel | ihr tut was, |
| 00:11:14,240 | 00:11:15,780 | Marcel | damit der User nicht 15 mal |
| 00:11:15,780 | 00:11:17,400 | Marcel | die Karte auflegt, weil es muss ihm auch klar sein, |
| 00:11:17,540 | 00:11:23,540 | Marcel | passiert jetzt eh nichts. Und da wir vier LEDs haben, glaube ich, ist es ganz praktisch, wenn man dann irgendwie so, wie so eine |
| 00:11:23,960 | 00:11:29,820 | Marcel | Sanduhr, oder diesen Kringel, den sich dreht, den kennt man jetzt fast jeden Betriebssystem, dass man irgendwas macht, wo einfach die |
| 00:11:30,060 | 00:11:38,880 | Marcel | LED im Uhrzeigersinn rumläuft und immer der Reihe nach die Lampen angeht, in Orange zum Beispiel, und dass man dann wie so eine Art |
| 00:11:39,240 | 00:11:41,600 | Marcel | Kringel hat, der sich dreht, das ist, glaube ich, intuitiv verständlich. |
| 00:11:44,600 | 00:11:48,400 | Marcel | Genau. Und dann am Ende gibt es eine Entscheidung: Ich darf rein oder ich darf nicht rein. |
| 00:11:50,660 | 00:11:53,780 | Marcel | Und da haben wir ja auch schon definiert: Auto öffnen ist klar. Man hört, dass es klack macht. |
| 00:11:54,780 | 00:11:56,000 | Marcel | Es blinkt zweimal grün. |
| 00:11:56,540 | 00:11:59,180 | Marcel | Karte wird abgelehnt. Aktuell glaube ich, es blinkt zweimal rot. |
| 00:11:59,680 | 00:12:02,300 | Marcel | Dann heißt es: Die Karte wurde abgelehnt. Darf nicht aufmachen. |
| 00:12:05,100 | 00:12:06,700 | Marcel | Das ist das Thema Aufsperren. |
| 00:12:09,680 | 00:12:12,980 | Marcel | Als Use Case schon mit eigentlich ein bisschen technischer Definition drumherum. |
| 00:12:15,380 | 00:12:24,200 | Marcel | Sprich im Rahmen des Aufsperrens haben wir eigentlich ein Backend-Service, den wir brauchen, nämlich Reservierungen abfragen. |
| 00:12:27,760 | 00:12:43,620 | Marcel | Der Rest ist intern. Wir haben ja den Sonderfall. Das Auto bekommt keine Antwort auf. Also es ist keine Reservierung da und es bekommt keine Antwort auf eine Reservierungsanfrage, weil es zum Beispiel in der Tiefgarage steht, kein Netz hat. |
| 00:12:45,060 | 00:12:49,640 | Marcel | ein Backend abgeschmiert ist, hoffentlich der absolut geringste Fall, aber es kriegt keine Antwort. |
| 00:12:50,739 | 00:12:54,040 | Marcel | Dann haben wir ja noch gesagt, es gibt die globale Whitelist. |
| 00:12:56,280 | 00:12:58,180 | Marcel | Und wenn er da nicht draufsteht, wird er abgelehnt. |
| 00:12:58,860 | 00:13:00,960 | Marcel | Mich halte es für nicht sinnvoll, dass man sagt, |
| 00:13:04,860 | 00:13:07,460 | Marcel | man fragt in dem Moment die Whitelist beim Server an, |
| 00:13:09,759 | 00:13:13,280 | Marcel | weil wenn ich eine Reservierungsanfrage nicht durchbringe, |
| 00:13:13,400 | 00:13:14,740 | Marcel | kriege ich auch eine Whitelistanfrage nicht durch. |
| 00:13:14,980 | 00:13:16,520 | Robert | Ja, genau. |
| 00:13:17,420 | 00:13:18,300 | Robert | Das ist bei dir. |
| 00:13:21,720 | 00:13:25,640 | Marcel | Dann ist Use Case Aufsperren abgeschlossen. |
| 00:13:28,380 | 00:13:29,500 | Marcel | Auto absperren. |
| 00:13:31,100 | 00:13:40,180 | Marcel | Das ist jetzt die spannende Frage, ob man das überhaupt nennenswert limitieren sollte. |
| 00:13:40,520 | 00:13:43,240 | Marcel | Ich glaube, ein Auto absperren sollte man einfach offen lassen. |
| 00:13:44,360 | 00:13:52,420 | Marcel | Das darf im Endeffekt jeder machen, der eine Karte draufhält, ob er reserviert hat oder nicht. |
| 00:13:52,950 | 00:13:53,360 | Marcel | Total egal. |
| 00:13:53,720 | 00:13:54,740 | Robert | Ich bin mir nicht sicher. |
| 00:13:56,320 | 00:13:57,520 | Marcel | Das müssen wir durchspielen. |
| 00:13:57,860 | 00:13:59,760 | Marcel | Gut, dann lass uns mal durchspielen, was kann passieren. |
| 00:13:59,850 | 00:14:01,380 | Marcel | Also, Use Case: Auto absperren. |
| 00:14:02,279 | 00:14:03,520 | Marcel | Wäre der zweite Use Case. |
| 00:14:05,520 | 00:14:10,760 | Robert | Also, was spricht dafür, dass jeder darf, und was spricht dagegen, dass jeder darf? |
| 00:14:11,440 | 00:14:13,760 | Marcel | Dafür spricht das ein sehr einfacher Prozess. |
| 00:14:15,340 | 00:14:16,100 | Marcel | In der ZK. |
| 00:14:16,920 | 00:14:18,960 | Marcel | Legt die Karte drauf und ist abgesperrt. |
| 00:14:19,280 | 00:14:19,980 | Marcel | Fertig. Gute Nacht. |
| 00:14:21,000 | 00:14:23,440 | Marcel | Wenn ich an einem Auto vorbeikomme und das Auto ist noch offen, |
| 00:14:23,580 | 00:14:24,320 | Marcel | ich sehe das... |
| 00:14:27,779 | 00:14:30,580 | Marcel | Ich würde es tatsächlich auf eine Karte in der Whitelist reduzieren. |
| 00:14:30,580 | 00:14:32,000 | Marcel | Man schaut so wie jetzt auch. |
| 00:14:32,920 | 00:14:35,420 | Marcel | Steht die Karte in der globalen Whitelist? Ja, dann darfst du zusperren. |
| 00:14:36,140 | 00:14:39,800 | Marcel | Dass man nicht mit einer Bankkarte absperren kann oder mit irgendjemandem, der vorbeigeht. |
| 00:14:41,200 | 00:14:42,860 | Marcel | im Endeffekt wäre selbst das aber die Frage |
| 00:14:43,760 | 00:14:44,520 | Marcel | Schritt zurück |
| 00:14:45,380 | 00:14:46,820 | Marcel | was kann passieren |
| 00:14:48,020 | 00:14:48,920 | Marcel | wenn ich das Auto |
| 00:14:49,550 | 00:14:50,460 | Marcel | einfach so absperre |
| 00:14:50,700 | 00:14:51,660 | Marcel | schlimmster Fall ist |
| 00:14:52,460 | 00:14:54,260 | Marcel | Worst Case Betrachtung |
| 00:14:54,860 | 00:14:55,820 | Marcel | vom Absperren heißt |
| 00:14:56,780 | 00:14:58,420 | Marcel | der eigentlich ausleihende Benutzer |
| 00:14:58,510 | 00:14:59,680 | Marcel | ist nur kurz ins Haus gegangen |
| 00:15:01,199 | 00:15:02,880 | Marcel | hat sein Geldbeutel, sein Handy |
| 00:15:03,740 | 00:15:04,640 | Marcel | und auch seine |
| 00:15:05,220 | 00:15:07,000 | Marcel | Fahrt-Zutrittskarte im Auto liegen |
| 00:15:07,250 | 00:15:09,260 | Marcel | es kommt jemand vorbei und sperrt das Auto ab |
| 00:15:10,700 | 00:15:17,300 | Marcel | Dann kommt er nicht mehr ins Auto rein und muss sich ein Handy anrufen. |
| 00:15:18,760 | 00:15:23,260 | Marcel | Jetzt ist die Frage: Nein, weil er normalerweise hat dann den Schlüssel dabei. |
| 00:15:25,680 | 00:15:26,280 | Marcel | Normalerweise. |
| 00:15:27,080 | 00:15:36,360 | Marcel | Er müsste quasi den Schlüssel im Auto liegen haben, plus Geldbeutel mit der Karte und dem Führerschein. |
| 00:15:37,690 | 00:15:41,140 | Marcel | Dann wäre es ein Problem. Selbst mit Handy würde das Auto nicht aufsperren können. |
| 00:15:41,310 | 00:15:44,000 | Marcel | Das wäre bloß technisches Hilfsmittel, um den Fahrzeug zu kontaktieren. |
| 00:15:45,000 | 00:15:47,020 | Marcel | Aber er könnte es nicht wieder aufsperren. |
| 00:15:50,600 | 00:15:57,660 | Marcel | Gut. Wahrscheinlichkeit, dass das passiert, dass jemand dann am Auto vorbei geht und einfach mal sein Geldbeutel auf diesen Chipkarten-Verser hält, ist sehr gering, aber da gebe ich dir recht, ist nicht schön. |
| 00:15:59,040 | 00:16:05,100 | Marcel | Deswegen glaube ich, es wäre sinnvoller, wenn man es auf die Whitelist beschränkt, dass nur Karten in der Whitelist das Auto zusperren können. |
| 00:16:06,220 | 00:16:07,900 | Robert | In welcher Whitelist? In der globalen? |
| 00:16:08,980 | 00:16:09,620 | Marcel | In der globalen. |
| 00:16:09,680 | 00:16:10,080 | Robert | Na ja, gut. |
| 00:16:16,620 | 00:16:17,260 | Marcel | Okay, also... |
| 00:16:17,280 | 00:16:18,560 | Marcel | einen Autoteiler mitglieden. |
| 00:16:19,640 | 00:16:22,880 | Robert | Okay, weil die dritte Alternative wäre ja nur, |
| 00:16:24,780 | 00:16:26,840 | Robert | Mitglieder aus der Liste, aus der |
| 00:16:29,920 | 00:16:33,380 | Robert | aus der lokalen Whiteliste, |
| 00:16:33,480 | 00:16:37,320 | Robert | die die Ideologisierung spezifisch ist, können zusperren. |
| 00:16:37,600 | 00:16:40,260 | Marcel | Hat aber den Nachteil, dass wenn ich meinen Termin |
| 00:16:40,960 | 00:16:44,740 | Marcel | eine Minute überziehe, oder mehr als die Kulanzzeit, die wir vielleicht noch einbauen, |
| 00:16:46,079 | 00:16:49,900 | Marcel | dass dann ich nicht mehr zusperren kann. |
| 00:16:51,600 | 00:16:53,860 | Marcel | Dann habe ich kein Handy dabei, dann kann ich auch die Buchung nicht verlängern. |
| 00:16:54,020 | 00:16:57,460 | Robert | Moment, Moment, das ist ein eigener Use Case, lass uns den separat besperren. |
| 00:16:58,020 | 00:16:58,200 | Robert | Okay. |
| 00:16:58,440 | 00:17:00,320 | Robert | Also was passiert, wenn ich... |
| 00:17:00,720 | 00:17:06,760 | Robert | Also erstens, was passiert, wenn ich überziehe? |
| 00:17:07,000 | 00:17:07,780 | Robert | Das ist das eine. |
| 00:17:08,699 | 00:17:12,760 | Robert | Und das andere ist, was passiert, wenn ich vorzeitig zusperre? |
| 00:17:12,920 | 00:17:14,540 | Robert | Auch den Use Case würde ich gerne betrachten. |
| 00:17:14,780 | 00:17:15,199 | Marcel | Okay. |
| 00:17:20,800 | 00:17:25,220 | Marcel | Du hast recht, also lass uns da mal eine Unterscheidung bei diesem Use Case reinmachen, technisch gesehen. |
| 00:17:25,819 | 00:17:27,680 | Marcel | Es gibt zwei Arten von Zusperren. |
| 00:17:27,689 | 00:17:31,820 | Marcel | Es gibt ein Zusperren, während eine aktive Reservierung vorliegt. |
| 00:17:32,480 | 00:17:34,560 | Marcel | Also ich zwischen Start und Ende der Reservierung bin. |
| 00:17:35,780 | 00:17:41,760 | Marcel | Da gebe ich dir recht, da macht es total Sinn, das Zusperren zu beschränken auf die Karten-IDs, |
| 00:17:42,620 | 00:17:44,380 | Marcel | die in der Whitelist des Termins sind. |
| 00:17:44,480 | 00:17:45,160 | Marcel | also der Reservierung. |
| 00:17:47,600 | 00:17:48,760 | Marcel | Also das ist der Use Case. |
| 00:17:48,960 | 00:17:52,880 | Marcel | Ich sperre ab innerhalb der von mir getätigten Reservierung. |
| 00:17:53,779 | 00:17:56,360 | Marcel | Dann nutzen wir die Whitelist des Termins |
| 00:17:56,520 | 00:17:58,020 | Marcel | und nur diese Karten, die da drinstehen, |
| 00:17:58,160 | 00:17:59,080 | Marcel | dürfen das Auto absperren. |
| 00:18:04,540 | 00:18:06,220 | Robert | Und was passiert, wenn ich überziehe? |
| 00:18:06,480 | 00:18:09,120 | Robert | Also, das ist eine generelle Frage. |
| 00:18:09,580 | 00:18:11,980 | Robert | Die ist jetzt mal unabhängig vom Zusperren. |
| 00:18:12,240 | 00:18:14,020 | Marcel | Ich würde es kurz beim Zusperren belassen. |
| 00:18:14,700 | 00:18:16,820 | Marcel | weil angenommen ich habe meinen Termin überzogen |
| 00:18:16,950 | 00:18:19,480 | Marcel | also sprich es gibt keine aktuelle Reservierung |
| 00:18:19,880 | 00:18:21,620 | Marcel | das ist ja die Konsequenz einer Überziehung |
| 00:18:23,040 | 00:18:25,840 | Marcel | also sprich ich habe ein Auto im Zustand geöffnet |
| 00:18:29,060 | 00:18:31,220 | Marcel | was passiert wenn jemand eine Karte drauflegt |
| 00:18:32,080 | 00:18:34,720 | Marcel | und ich bin der Meinung dass es dann total egal ist |
| 00:18:34,750 | 00:18:36,120 | Marcel | wenn keine Reservierung vorliegt |
| 00:18:36,160 | 00:18:39,360 | Marcel | es ist ganz egal welche Karte draufgelegt wird |
| 00:18:39,570 | 00:18:41,900 | Marcel | ich würde vielleicht sogar sagen noch nicht mal die White List |
| 00:18:42,440 | 00:18:44,440 | Marcel | die globale Whitelist verwenden, sondern ich würde sagen, |
| 00:18:44,500 | 00:18:46,520 | Marcel | es ist ganz egal, welche NFC-Karte draufgelegt wird. |
| 00:18:47,940 | 00:18:48,440 | Marcel | Die Nummer wird |
| 00:18:48,540 | 00:18:50,240 | Marcel | weggeschrieben und es wird zugesperrt, das Auto. |
| 00:18:55,700 | 00:18:56,100 | Robert | Okay. |
| 00:19:00,580 | 00:19:00,740 | Marcel | Weil |
| 00:19:05,100 | 00:19:06,820 | Robert | Also ich würde es auf jeden Fall auf die globale |
| 00:19:06,960 | 00:19:07,820 | Robert | Whitelist beschwenken. |
| 00:19:08,200 | 00:19:08,940 | Marcel | Okay, können wir machen. |
| 00:19:09,240 | 00:19:17,900 | Robert | Ich habe keine Vereinsweite Weitlist, weil wieso sollte ich irgendeine IC-Karte zum Zusperren zulassen? |
| 00:19:18,340 | 00:19:19,340 | Robert | Das macht für mich keinen Sinn. |
| 00:19:20,159 | 00:19:30,060 | Marcel | Ich sehe damit keine Gefahr, weil ja im Endeffekt das Risiko, was bestehen würde, ist, dass jemand keinen Termin hatte, das Auto offen gelassen hat und sein Zeug drin liegen lässt. |
| 00:19:31,320 | 00:19:37,100 | Robert | Ne, das Risiko ist, ich bin irgendwo, schaff's nicht rechtzeitig, in 5 Minuten bin ich da, |
| 00:19:37,170 | 00:19:43,140 | Robert | aber ich hab's vergessen zu verlängern, aber mir brissiert's, deswegen vergesse ich einen |
| 00:19:43,200 | 00:19:48,880 | Robert | Schlüssel und ein Ding im Auto, renn irgendwo rein und in der Zeit kommt irgendein Scherzkeks |
| 00:19:49,420 | 00:19:55,020 | Robert | und tut einfach nur seine Kreditkarten oder was auch immer vor dem Auto halten, das Auto sperrt zu |
| 00:19:55,270 | 00:19:57,280 | Robert | und ich kann das Auto nicht mehr zurückfahren damit. |
| 00:19:59,480 | 00:20:02,800 | Marcel | Also, ja, ich habe kein Problem damit, dass da auf die Whiteliste zu beschränken. |
| 00:20:02,880 | 00:20:07,220 | Marcel | Da hast du schon recht. Die Wahrscheinlichkeit ist sehr, sehr gering, aber das Ärgernis ist in diesem speziellen Fall sehr, sehr hoch. |
| 00:20:09,220 | 00:20:13,580 | Marcel | Und der Aufwand, der auch dahinter steht, weil da muss irgendjemand von dem Autoteilerverein dorthin fahren. |
| 00:20:14,840 | 00:20:20,460 | Marcel | Ich wollte nur noch mal gerade stellen, es gibt ja nicht die Vereinsweite Whitelist, es gibt die Sharepad-weite Whitelist. |
| 00:20:21,180 | 00:20:24,140 | Marcel | Es gibt keine vereinsspezifische Whitelist. |
| 00:20:27,820 | 00:20:29,700 | Robert | die ZK bekommt von mir |
| 00:20:29,840 | 00:20:30,960 | Robert | eine globale Whitelist. |
| 00:20:31,680 | 00:20:33,360 | Robert | Wen ich frei schreibe, das müssen wir |
| 00:20:33,540 | 00:20:34,500 | Robert | separat nochmal festlegen. |
| 00:20:34,640 | 00:20:36,860 | Marcel | Passt, gut. Aber es gibt, genau, aber es wichtig ist, |
| 00:20:36,900 | 00:20:39,220 | Marcel | es gibt die globale Whiteliste auf der ZK, |
| 00:20:39,240 | 00:20:41,200 | Marcel | du hast recht, aus Sicht der ZK ist es einfach nur |
| 00:20:42,279 | 00:20:43,080 | Marcel | Termins-Whitelist, |
| 00:20:43,500 | 00:20:44,320 | Marcel | Gesamt-Whitelist. |
| 00:20:45,500 | 00:20:47,500 | Marcel | Ob das dann nur die von dem Verein sind |
| 00:20:47,660 | 00:20:49,060 | Marcel | oder ob das die von allen Vereinen sind |
| 00:20:49,140 | 00:20:51,060 | Marcel | oder allen Vereinen mit Quernutzer-Vereinbarung, |
| 00:20:51,560 | 00:20:52,620 | Marcel | liegt in deinen Händen. |
| 00:20:53,000 | 00:20:53,800 | Marcel | Ja, vollkommen richtig. |
| 00:20:55,380 | 00:20:55,500 | Marcel | Gut. |
| 00:20:56,280 | 00:21:01,200 | Robert | Aber das ist ja das Elegante an der Lösung, dass wir die ZK außen vor lassen. |
| 00:21:01,740 | 00:21:04,900 | Marcel | Ja, das Schöne wird, wir kriegen die Trennung sauber hin. |
| 00:21:06,300 | 00:21:09,940 | Marcel | Backend, Frontend. Und das Frontend sollte normal möglichst wenig Intelligenz haben. |
| 00:21:11,100 | 00:21:11,300 | Robert | Genau. |
| 00:21:11,830 | 00:21:16,980 | Marcel | Und wenig Entscheidungsspielraum. Das macht alles das Backend und sagt dem Frontend nur ist gut, ist nicht gut. |
| 00:21:18,060 | 00:21:18,240 | Marcel | Genau. |
| 00:21:18,660 | 00:21:19,140 | Marcel | Ja, okay. |
| 00:21:19,980 | 00:21:24,060 | Marcel | Also den Fall zusperren haben wir gesagt, wenn der Termin noch aktiv ist. |
| 00:21:24,980 | 00:21:26,420 | Marcel | Also Use Case 2a sozusagen. |
| 00:21:27,340 | 00:21:28,140 | Marcel | Termin ist noch aktiv. |
| 00:21:28,780 | 00:21:32,180 | Marcel | Meine Reservierung dann, oder es ist noch ein Termin aktiv. |
| 00:21:33,040 | 00:21:36,360 | Marcel | Dann kann nur der zusperren, der auf der Termin-Wildlist steht. |
| 00:21:38,340 | 00:21:45,080 | Marcel | Ist der Termin vorbei, es gibt keine aktuelle Reservierung mehr, |
| 00:21:45,720 | 00:21:50,200 | Marcel | dann kann jeder, der auf der globalen Whitlist steht, das Auto zusperren. |
| 00:21:51,880 | 00:21:54,020 | Marcel | Jetzt bleiben wir noch beim Zusperren. |
| 00:21:59,560 | 00:22:00,440 | Marcel | Fall 2C |
| 00:22:01,740 | 00:22:02,660 | Marcel | Ich habe das Auto |
| 00:22:02,860 | 00:22:04,240 | Marcel | für 2 Stunden gebucht. |
| 00:22:06,100 | 00:22:07,080 | Marcel | Bin nach einer halben Stunde |
| 00:22:07,230 | 00:22:08,780 | Marcel | wieder da, weil |
| 00:22:10,140 | 00:22:11,160 | Marcel | ist was schief gegangen. |
| 00:22:11,590 | 00:22:13,500 | Marcel | Oder ich fahre erst gar nicht los, |
| 00:22:13,790 | 00:22:14,980 | Marcel | weil ich sage, bringt nichts. |
| 00:22:15,050 | 00:22:17,040 | Marcel | Eine halbe Stunde vorher wurde mir der Termin abgesagt. |
| 00:22:17,150 | 00:22:17,960 | Marcel | Ich brauche das Auto nicht. |
| 00:22:18,150 | 00:22:19,080 | Marcel | Ich lösche die Buchung. |
| 00:22:20,580 | 00:22:23,000 | Marcel | Dann kommt jemand her und sagt, voll geil, ich brauche das Auto. |
| 00:22:23,820 | 00:22:25,320 | Marcel | bucht sich selbst das Auto neu. |
| 00:22:29,460 | 00:22:33,840 | Marcel | Dann habe ich in der ZK eine Buchung für jetzt auf Marcel Meier |
| 00:22:35,020 | 00:22:38,020 | Marcel | und im Sharepad eine Buchung auf Robert Hölzl. |
| 00:22:40,020 | 00:22:41,800 | Marcel | Dann geht der Robert Hölzl hin zu dem Auto. |
| 00:22:42,440 | 00:22:46,740 | Marcel | Die ZK sagt, ah ja, sehr schön, ich habe eine Buchung für Marcel Meier. |
| 00:22:46,900 | 00:22:48,280 | Marcel | Deine Karte steht nicht auf der Weitlist. |
| 00:22:48,500 | 00:22:49,640 | Marcel | Tut mir leid, Robert, du darfst nicht rein. |
| 00:22:50,820 | 00:22:55,740 | Marcel | Also insofern ist es eigentlich ein 1B und ein 2C als Use Case. |
| 00:22:55,920 | 00:22:58,440 | Marcel | Das geht ja ums Aufsperren und ums Zusperren in zweiter Instanz. |
| 00:22:59,060 | 00:23:01,260 | Marcel | Jetzt in dem Fall habe ich gesagt, ich studiere die Buchen komplett. |
| 00:23:01,360 | 00:23:03,860 | Marcel | Das kann aber auch sein, dass ich habe sie für 8 Stunden reserviert, das Auto, |
| 00:23:05,240 | 00:23:10,080 | Marcel | bin 2 Stunden früh wieder zurück, gebe die 2 Stunden frei und du buchst das Auto in der Zwischenzeit. |
| 00:23:11,620 | 00:23:16,400 | Marcel | Dann hättest du aber tatsächlich nur ein Problem, wenn ich dir das Auto übergebe, |
| 00:23:16,470 | 00:23:19,000 | Marcel | weil wir fliegende Übergabe machen, musst du es nicht aufsperren, |
| 00:23:19,100 | 00:23:24,180 | Marcel | bekommst du es von mir aufgesperrt übergeben, steigst ein, fährst los und kannst es dann aber am Ende nicht zusperren, |
| 00:23:24,260 | 00:23:28,660 | Marcel | nachdem du deine halbe Stunde Einkünfe gemacht hast. Weil es ja noch mein Termin in der ZK kam. |
| 00:23:29,520 | 00:23:34,220 | Robert | Ja, wir müssen einfach generell das Diagramm noch einmal ändern vom letzten Mal. |
| 00:23:35,340 | 00:23:47,660 | Robert | Du musst immer, egal ob die Reservierung da ist, falsch ist, dann haben wir eben den Diagramm jetzt gesagt ablehnen. |
| 00:23:48,340 | 00:23:51,340 | Robert | Wir müssen stattdessen immer beim Server aufbauen. |
| 00:23:55,980 | 00:24:04,900 | Marcel | Du musst immer im Ablehnungsfall, selbst wenn es einen Termin gibt, aber die Karte, die draufgelegt wurde, steht nicht auf der Whitelist, trotzdem jetzt anfragen. |
| 00:24:05,200 | 00:24:06,580 | Marcel | Das ist eine wichtige Konsequenz daraus. |
| 00:24:08,459 | 00:24:09,840 | Robert | Noch schlimmer, Marcel. |
| 00:24:10,420 | 00:24:15,160 | Robert | Wir müssen selbst wenn sie passt, die Buchung, eigentlich beim Server aufbauen. |
| 00:24:16,360 | 00:24:21,980 | Robert | Weil du kannst ja die Buchung stornieren und dann zum Auto gehen. |
| 00:24:22,400 | 00:24:24,840 | Marcel | Ja, hast recht. |
| 00:24:25,280 | 00:24:29,700 | Marcel | Das heißt, wir müssen eigentlich als allerersten Schritt davor, |
| 00:24:30,020 | 00:24:32,360 | Marcel | ist die Reservierung lokal vorhanden. |
| 00:24:33,280 | 00:24:38,100 | Marcel | Davor müssen wir eigentlich einen Schritt reinbauen, Reservierungen aktualisieren. |
| 00:24:42,200 | 00:24:44,639 | Marcel | Heißt aber, dass wir natürlich bei jedem Auf- und Zusperren |
| 00:24:45,460 | 00:24:46,640 | Marcel | und Request an deinen Surfer schicken. |
| 00:24:47,590 | 00:24:48,620 | Robert | Ja, und das ist scheiße. |
| 00:24:50,590 | 00:24:51,960 | Robert | Da verlieren wir jede Menge Zeit. |
| 00:24:52,100 | 00:24:53,440 | Robert | Wir müssen das Modem aufwäckern, |
| 00:24:53,460 | 00:24:54,740 | Robert | wir müssen ein Netz kriegen. |
| 00:24:54,960 | 00:24:57,860 | Marcel | Beim Aufsperren müssen wir jedes Mal eine Anfrage an den Surfer schicken. |
| 00:24:57,870 | 00:24:58,120 | Marcel | So rum. |
| 00:24:59,410 | 00:24:59,560 | Robert | Ja. |
| 00:25:01,870 | 00:25:02,920 | Robert | Das ist richtig der Bad. |
| 00:25:03,560 | 00:25:03,740 | Marcel | Mhm. |
| 00:25:04,960 | 00:25:07,639 | Robert | Okay, da müssen wir nochmal schauen, |
| 00:25:13,740 | 00:25:14,620 | Robert | was technisch... |
| 00:25:14,640 | 00:25:19,580 | Robert | Aufwecker und eine Anfrage an Server machen, wenn ich nur ganz minimal eine Anfrage mache, |
| 00:25:20,060 | 00:25:24,800 | Robert | was ist da die kürzeste Zeit und falls die nicht akzeptabel ist, |
| 00:25:25,080 | 00:25:30,400 | Robert | was sind die Möglichkeiten einer Push-Notification, wie auch immer das dann ausschaut. |
| 00:25:30,580 | 00:25:32,080 | Marcel | Also ich hätte einen anderen Vorschlag kurz. |
| 00:25:33,000 | 00:25:38,360 | Marcel | Ich hätte nämlich den Vorschlag, dass die ZK ja weiß, wann kommt der Nächste zum Aufsperren |
| 00:25:40,200 | 00:25:44,420 | Marcel | und einfach in der Timeloop prüft, wenn in 15 Minuten ein Termin anfängt, |
| 00:25:44,560 | 00:25:46,100 | Marcel | frage ich jetzt nochmal den Termin ab. |
| 00:25:47,160 | 00:25:48,500 | Marcel | Ja, das ist eine gute Idee. |
| 00:25:49,200 | 00:25:50,620 | Marcel | Damit habe ich das getrennt |
| 00:25:50,680 | 00:25:51,820 | Marcel | von der User-Interaktion. |
| 00:25:52,440 | 00:25:54,700 | Marcel | Und dann ist die Wahrscheinlichkeit, dass ein gelöschter Termin |
| 00:25:55,639 | 00:25:56,440 | Marcel | nicht, oder |
| 00:25:56,660 | 00:25:58,420 | Marcel | fünf Minuten vorher, eine Minute vorher, reicht ja, |
| 00:25:58,420 | 00:26:00,800 | Marcel | weil er kann eine Minute vorher, kann er, darf er nicht aufsperren. |
| 00:26:02,840 | 00:26:04,320 | Marcel | Also, genauer gesagt, |
| 00:26:04,420 | 00:26:06,460 | Marcel | wir werden einen Hauch von Kulanzzeit einbauen müssen, |
| 00:26:07,180 | 00:26:08,360 | Marcel | weil auf die Minute genau |
| 00:26:08,740 | 00:26:10,280 | Marcel | ist, dafür gehen die Uhren falsch, |
| 00:26:10,420 | 00:26:11,280 | Marcel | also irgendwie so fünf Minuten |
| 00:26:12,320 | 00:26:14,480 | Marcel | Kulanz muss man einbauen, und der muss halt |
| 00:26:14,500 | 00:26:18,900 | Marcel | eine Minute vor der ersten Möglichkeit, dass der Benutzer aufsperren kann, der |
| 00:26:18,920 | 00:26:20,980 | Marcel | reserviert hat, muss er noch mal aktualisieren. |
| 00:26:30,460 | 00:26:35,740 | Robert | Okay, das wird heißen, genau, wir werden die Minute oder was auch immer |
| 00:26:38,100 | 00:26:50,320 | Robert | Aktualisieren. Wie lange, ja oder wir werden dann tatsächlich im Viertelstunden-Takt aktualisieren, |
| 00:26:50,480 | 00:26:58,320 | Robert | so lange bis die Benutzer kommt. Also wenn da eine Reservierung ist, dass ich sozusagen alle Viertelstunden sage, |
| 00:26:59,220 | 00:27:00,620 | Robert | ja wie, du bist immer noch nicht da, |
| 00:27:00,860 | 00:27:02,000 | Robert | okay, dann schauen wir nach, ob das |
| 00:27:02,320 | 00:27:03,040 | Robert | aktuell ist. |
| 00:27:06,460 | 00:27:08,400 | Robert | Gut, das ist eine super Idee, aber da müssen wir |
| 00:27:08,420 | 00:27:10,360 | Robert | schauen, was die Datenverbräuche sind. |
| 00:27:10,980 | 00:27:12,520 | Marcel | Ja, und da müssen wir auch schauen, |
| 00:27:12,640 | 00:27:14,460 | Marcel | dann macht es unter dem Ständen nämlich doch Sinn, zu sagen, |
| 00:27:14,600 | 00:27:16,500 | Marcel | man splittet den, gibt mir die Termine |
| 00:27:16,560 | 00:27:18,320 | Marcel | Service in ein, gibt mir die |
| 00:27:18,440 | 00:27:20,360 | Marcel | nächsten 14 Tage und gibt mir die Termine |
| 00:27:20,440 | 00:27:21,840 | Robert | plus ein bisschen Stunde. Jetzt. |
| 00:27:22,380 | 00:27:24,400 | Robert | Ja, genau. Weil dann habe ich |
| 00:27:24,580 | 00:27:26,200 | Marcel | natürlich mikroskopische Datenmengen, |
| 00:27:26,280 | 00:27:26,900 | Marcel | die ich übertrage. |
| 00:27:28,340 | 00:27:32,840 | Robert | Ja, also wir haben halt generell momentan Rest als Protokoll. |
| 00:27:34,180 | 00:27:37,060 | Robert | Ich würde, wenn es irgendwie geht, gerne dabei bleiben. |
| 00:27:38,720 | 00:27:42,860 | Robert | Aber das müsste man einfach überschlagen, was die Datenmengen sind, die zu erwarten sind. |
| 00:27:47,620 | 00:27:54,000 | Marcel | Das würde ich jetzt erstmal entspannt auf uns zukommen lassen, weil so teuer sind die Datenvolumine ja jetzt auch nicht, die du dazu kaufst. |
| 00:27:54,020 | 00:27:56,000 | Marcel | Das sind pro 500 MB 10 Euro, glaube ich. |
| 00:27:56,680 | 00:27:58,520 | Marcel | Und wenn man das mal ein bisschen beobachtet, |
| 00:27:58,660 | 00:28:02,660 | Marcel | selbst wenn du da im 5-Minuten-Takt abfragst, dann. |
| 00:28:03,820 | 00:28:04,740 | Marcel | Oder im Minuten-Takt. |
| 00:28:04,740 | 00:28:05,580 | Robert | Also im Viertelstunden-Takt. |
| 00:28:05,880 | 00:28:10,280 | Robert | Sollen wir Viertelstunden, das wären ungefähr 100 Abfragen pro Tag, mal 365. |
| 00:28:11,500 | 00:28:13,620 | Robert | Das sind 36.000 Abfragen im Jahr. |
| 00:28:16,440 | 00:28:21,540 | Robert | Sollen wir pro Abfrage, lass ein Kilobyte sein, dann sind 36 MB fast. |
| 00:28:22,380 | 00:28:22,560 | Robert | Okay. |
| 00:28:22,740 | 00:28:40,400 | Marcel | Ja, und die Frage ist eben, machst du immer permanent eine, also ich würde gar nicht dauerhaft eine Abfrage machen, ich würde das nur machen während eines Reservierungszeitraums, also ab 5 Minuten davor bis Ende einer Reservierung, aber die Zeit, wo das Auto nicht reserviert ist, gibt es keine Abfrage, außer jemand legt eine Karte drauf. |
| 00:28:41,210 | 00:28:49,000 | Marcel | Weil das ist argumentierbar, dass ich sage, wenn ich spontan eine Buchung eintrage, dann muss ich halt am Auto stehen und mir halt nochmal die 2 Minuten mehr Zeit nehmen oder die Minute. |
| 00:28:49,500 | 00:28:52,440 | Robert | Ja, das war nur eine Werftpress-Abschätzung. |
| 00:28:52,580 | 00:28:53,260 | Robert | Passt, verstehe. |
| 00:28:55,140 | 00:29:03,380 | Robert | Weil wenn du sagen 500 Megabyte und 36 Megabyte, wenn wir durchgehend reserviert hätten und keiner leitet es aus, das ist der Worst-Case, dann haben wir immer einen grünen Bereich. |
| 00:29:03,500 | 00:29:09,860 | Marcel | Genau, weil in dem Moment, wo das Auto aufgesperrt ist, muss ich nicht mehr abfragen und in dem Moment, wo keine Reservierung da ist, muss ich die Abfrage auch nicht machen. |
| 00:29:10,920 | 00:29:11,000 | Marcel | Genau. |
| 00:29:11,400 | 00:29:15,140 | Marcel | Das heißt, es geht nur um die Zeit, wo das Auto reserviert ist, aber zugesperrt. |
| 00:29:15,840 | 00:29:19,400 | Marcel | Nur diese Zeit, da müssen wir regelmäßig die Reservierungen anfragen. |
| 00:29:19,980 | 00:29:21,120 | Marcel | und mit 1KB |
| 00:29:21,640 | 00:29:22,680 | Marcel | klar ist ein Aufwand dabei |
| 00:29:23,070 | 00:29:23,900 | Marcel | aber ähm |
| 00:29:24,950 | 00:29:26,060 | Marcel | ich glaube das ist eine reine |
| 00:29:26,900 | 00:29:28,540 | Marcel | ich würde mich jetzt mal kurz interessieren |
| 00:29:30,000 | 00:29:32,280 | Marcel | äh ich gebe die Frage mal kurz an Cloud weiter |
| 00:29:33,960 | 00:29:36,260 | Robert | ja es ist eine interessante Frage ob ich mit dem Kilobyte |
| 00:29:36,720 | 00:29:38,580 | Robert | mit dem Kilobyte Nr. 10er Potenz vernehme |
| 00:29:39,200 | 00:29:41,200 | Robert | weil das hat jetzt aus dem Kopf geschüttelt |
| 00:29:41,260 | 00:29:43,180 | Robert | aus dem Ärmel geschüttelt ohne Wissen |
| 00:29:44,040 | 00:29:47,480 | Marcel | warte mal ich diktiere bloß die Frage schon da rein |
| 00:29:47,980 | 00:30:14,540 | Marcel | Wenn ich eine TCP-Anfrage über eine REST-API stelle und diese Anfrage keine Nutzdaten enthält, wie viel Kilobyte Daten fallen für eine leere REST-Anfrage an auf Empfängerseite? |
| 00:30:19,600 | 00:30:20,120 | Robert | Was? |
| 00:30:21,280 | 00:30:29,160 | Robert | Du musst HTTPS sagen. Weil HTTP ist natürlich sehr klein, aber HTTPS, da ist TLS und bei TLS, da schreibt es dann alles aus. |
| 00:30:29,270 | 00:30:32,460 | Marcel | Ok, also für eine TCP an? |
| 00:30:32,460 | 00:30:35,760 | Robert | Also die Pro-Verbindung fallen einmalig ein paar Kilobyte Handshake-Garten an. |
| 00:30:36,720 | 00:30:42,200 | Robert | Genau, also das ist der richtige Server, das haben wir nicht gegeben, aber der Handshake ist das Problem. |
| 00:30:42,760 | 00:30:48,800 | Robert | Aber was wir machen können, halt, wir können tatsächlich eine HTTP-Anfrage machen, die kann unverschlüsselt sein. |
| 00:30:49,640 | 00:30:50,760 | Robert | Das machen wir, genau. |
| 00:30:51,300 | 00:30:57,180 | Robert | Das heißt, wir machen eine unverschlüsselte HTTP-Anfrage, da steht nur drin, es gibt Neuigkeiten. |
| 00:30:58,460 | 00:31:01,780 | Robert | Und wenn dies drin steht, dann machen wir die verschlüsselte Anfrage. |
| 00:31:05,560 | 00:31:10,700 | Robert | Es geht uns ja nicht darum, die Nutzdaten wieder zu vermitteln, sondern nur zu wissen, was wir machen sollen. |
| 00:31:12,160 | 00:31:14,520 | Marcel | Okay, ja, das heißt, du machst eine HTTP-Anfrage. |
| 00:31:16,380 | 00:31:18,120 | Marcel | Also, manchmal noch kurz... |
| 00:31:30,040 | 00:31:33,580 | Marcel | Genau, das heißt, du sagst, wir machen dann eine Viertelstunde eine Mini-Anfrage, die nur sagt, |
| 00:31:36,760 | 00:31:37,520 | Marcel | ist eine Buchung da? |
| 00:31:43,040 | 00:31:47,140 | Marcel | Und wenn einer da ist, dann schickt er die andere noch mal hinterher. |
| 00:31:49,740 | 00:31:52,080 | Marcel | Das Konzept habe ich gerade nicht verstanden, was du gesagt hast. |
| 00:31:53,360 | 00:31:56,500 | Robert | Also die Anfrage wäre, gab es eine Änderung? |
| 00:31:59,000 | 00:32:03,300 | Marcel | Stimmt, gab es eine Änderung seit Zeitstempeln der letzten Sync. |
| 00:32:04,420 | 00:32:07,340 | Marcel | Also wann habe ich das letzte Mal den Gibt-mir-alle-Termine abgefragt? |
| 00:32:08,340 | 00:32:17,840 | Marcel | Und wenn es keine Änderung gab, seitdem bei dieser Ressource der Buchung in den nächsten 24 Stunden, |
| 00:32:18,440 | 00:32:21,840 | Marcel | wenn es in den nächsten 24 Stunden keine Änderung an Terminen gab, dann... |
| 00:32:22,240 | 00:32:23,600 | Marcel | Ne, noch einfacher, stimmt, das ist vollkommen recht. |
| 00:32:24,080 | 00:32:28,700 | Marcel | Wir können dir diese Anfrage immer zuerst vorschicken, können sagen, gibt es eine Reservierungsänderung? |
| 00:32:29,980 | 00:32:33,380 | Marcel | Ja, dann lädt er einfach das runter. Nein, er lädt nichts runter. |
| 00:32:34,080 | 00:32:38,100 | Marcel | Und diese Anfrage machen wir gar nicht in dem Daily Ping, sondern die machen wir wirklich alle 5 Minuten. |
| 00:32:38,740 | 00:32:40,500 | Marcel | Weil das ist eine mikroskopisch kleine Anfrage. |
| 00:32:42,860 | 00:32:46,440 | Robert | Ja, alle 5 Minuten ist schwierig, weil es geht ja nicht nur um die Datenmenge, sondern |
| 00:32:46,480 | 00:32:47,480 | Robert | auch um den Stromverbrauch. |
| 00:32:47,880 | 00:32:49,300 | Marcel | Ja und um deinen Server, du hast recht, ja. |
| 00:32:50,900 | 00:33:00,760 | Robert | Ja, ich glaube der wäre nicht das Problem, aber um die Stromverbrauche im Auto, also |
| 00:33:01,500 | 00:33:04,920 | Robert | ich würde da schon, das ist nicht zu viel machen. |
| 00:33:05,660 | 00:33:16,480 | Robert | Also schon, zuvor hast du schon gesagt, es reicht ja, wenn man zwischen Reservierung und Aufsperrung die Zeiten einen vierten Tag anträgt. |
| 00:33:16,800 | 00:33:22,100 | Robert | Und wie du schon gesagt hast, es geht nur darum, dass der Benutzer nicht warten muss. |
| 00:33:24,060 | 00:33:32,160 | Marcel | Ja, beziehungsweise dass eben, wenn eine Buchung gelöscht wurde, sichergestellt wird, dass trotz gelöschter Buchung der User nicht trotzdem rein kann. |
| 00:33:33,340 | 00:33:37,460 | Marcel | dass ich nicht eine Buchung mache, die lösche und dann trotzdem das Auto aufsperren kann. |
| 00:33:38,880 | 00:33:39,780 | Marcel | Darum geht es im Endeffekt. |
| 00:33:39,780 | 00:33:47,260 | Marcel | Das heißt, ich würde eigentlich das Zeitraß sehr kleiner setzen. |
| 00:33:47,390 | 00:33:53,180 | Marcel | Ich würde vielleicht sogar diese Mini-Anfrage im Minuten- oder 5-Minuten-Takt machen, |
| 00:33:53,180 | 00:33:58,600 | Marcel | weil du machst sie ja nur beginnend ab, kurz vor, ich könnte frühestens aufsperren, |
| 00:33:58,600 | 00:34:01,240 | Marcel | bis es wird aufgesperrt, das Auto. |
| 00:34:01,240 | 00:34:02,440 | Marcel | Da mache ich diese Anfragen ja nur. |
| 00:34:02,880 | 00:34:09,280 | Marcel | Und die Anfrage ist ja erstmal simpel und einfach und in 90% der Fälle reicht die einfache Anfrage mit: "Gibt es eine Änderung?" |
| 00:34:13,859 | 00:34:17,120 | Marcel | Kannst ja ganz konkret sagen: "Gibt es eine Änderung beim nächsten Termin?" |
| 00:34:19,840 | 00:34:22,120 | Marcel | Ja, wobei das ist schwieriger für dich wahrscheinlich. |
| 00:34:23,159 | 00:34:28,000 | Robert | Wie man die Anfrage genau macht, wenn wir das mal hier stellen: |
| 00:34:28,980 | 00:34:36,379 | Robert | Das Entscheidende ist für mich weniger, wie sie ausschaut und das Entscheidende ist, wie viel Strom brauchen wir in der Zeit. |
| 00:34:39,659 | 00:34:45,500 | Robert | Die Anfrage kommt ja kurz, aber bis wir den Prozessor aufgelegt haben, bis wir uns ins Netz eingeloggt haben. |
| 00:34:46,720 | 00:34:52,220 | Marcel | Also das ins Netz einloggen kostet Zeit, aber tatsächlich ist, wenn das Ding, also der Strom, |
| 00:34:52,879 | 00:34:58,380 | Marcel | Da muss man Finn nochmal fragen, ob er das nochmal verifizieren kann, aber wie ich die alte Platine hier hatte, nicht die neue, |
| 00:34:59,720 | 00:35:04,280 | Marcel | habe ich hiermit permanent angearbeitet. Also ich habe weder das Monom ausgemacht, noch irgendwas. |
| 00:35:04,440 | 00:35:09,520 | Marcel | Und wir waren trotzdem so in der Kategorie 40mA für den Arduino. |
| 00:35:11,640 | 00:35:12,440 | Marcel | Das war nicht viel... |
| 00:35:12,440 | 00:35:13,980 | Robert | Ja eben, ab 40mA finde ich viel. |
| 00:35:16,240 | 00:35:17,400 | Robert | Das finde ich jetzt nicht. |
| 00:35:18,480 | 00:35:21,060 | Robert | Moment, bei was? Bei 5V oder bei? |
| 00:35:21,840 | 00:35:23,500 | Marcel | Äh, an 5V, ja. |
| 00:35:24,860 | 00:35:28,560 | Robert | 5V, genau, also 200 Watt. |
| 00:35:29,440 | 00:35:29,980 | Robert | Milliwatt. |
| 00:35:29,980 | 00:35:32,740 | Robert | Wenn ich die eine Stunde lang... 200mW, entschuldigung. |
| 00:35:33,620 | 00:35:34,380 | Robert | 200mW, genau. |
| 00:35:35,470 | 00:35:39,280 | Robert | Mal 24, dann haben wir pro Tag |
| 00:35:41,160 | 00:35:43,040 | Robert | 4,8 kWh. |
| 00:35:43,580 | 00:35:44,100 | Robert | Wattstunden. |
| 00:35:44,450 | 00:35:46,460 | Robert | Ne, Wattstunden. Wattstunden, genau. |
| 00:35:46,860 | 00:35:48,620 | Robert | 4,8 Wh |
| 00:35:50,380 | 00:35:51,920 | Robert | und wie gehört die Autobatterie? |
| 00:35:52,640 | 00:35:56,600 | Marcel | Autobatterie, die kleinen haben, wenn ich es richtig im Kopf habe, |
| 00:35:57,520 | 00:36:00,980 | Marcel | die ganz ganz kleinen, so 20 Amperestunden auf 12 Volt |
| 00:36:01,880 | 00:36:05,420 | Marcel | das heißt, die haben 20 x 12 haben 240 Wh |
| 00:36:08,320 | 00:36:12,580 | Marcel | also mit dem Verbrauch, den wir ausgerechnet haben, haben wir ausgerechnet, dass die |
| 00:36:12,660 | 00:36:15,880 | Marcel | Batterie eines Kleinwagens, wenn das Auto einfach nur steht, |
| 00:36:16,650 | 00:36:18,720 | Marcel | nach zwei Wochen noch nicht halb leer war. |
| 00:36:18,840 | 00:36:19,980 | Marcel | Aber das war so die Vorgabe, die wir hatten. |
| 00:36:20,030 | 00:36:22,120 | Marcel | Ich glaube nach vier Wochen wäre es in Richtung halb leer gegangen. |
| 00:36:22,280 | 00:36:23,100 | Marcel | Das war das, was okay ist. |
| 00:36:24,430 | 00:36:27,620 | Marcel | Das heißt, der Stromverbrauch ist nicht hoch. |
| 00:36:27,690 | 00:36:28,780 | Marcel | Das heißt, wir haben Stromverbrauch, |
| 00:36:29,980 | 00:36:33,300 | Marcel | zumal wir ja die Abfragen nur machen, wenn eine Buchung ansteht. |
| 00:36:34,080 | 00:36:34,820 | Marcel | Das heißt, wenn |
| 00:36:35,440 | 00:36:37,920 | Marcel | theoretisch jede Buchung abgesagt wird, hätten wir ein Problem. |
| 00:36:38,720 | 00:36:40,900 | Marcel | Weil dann macht er das, solange die Buchung aktiv war, |
| 00:36:41,040 | 00:36:42,280 | Marcel | und danach hört aber auch wieder auf. |
| 00:36:42,780 | 00:36:45,600 | Marcel | Das heißt, im schlimmsten Fall, blödes Szenario kann natürlich sein, |
| 00:36:47,360 | 00:36:51,220 | Marcel | es ist eine Buchung, reserviert für Fahrt, Werkstatt, |
| 00:36:51,320 | 00:36:54,860 | Marcel | vier Wochen, weil wir was klären müssen, an dem Auto neu folieren wollen, |
| 00:36:55,240 | 00:36:57,520 | Marcel | und dann macht der vier Wochen alle fünf Minuten eine Anfrage. |
| 00:36:59,080 | 00:37:00,080 | Marcel | Das ist ein blödes Szenario. |
| 00:37:02,340 | 00:37:07,180 | Marcel | Aber wäre auch nur bei einem Kleinwagen und vier Wochen wirklich ein Thema. |
| 00:37:07,240 | 00:37:10,000 | Marcel | Und wenn ein Autoteilerverein das Auto mal vier Wochen stehen hat, |
| 00:37:10,740 | 00:37:13,800 | Marcel | dann muss man schauen, ob man da vielleicht eine Lösung findet. |
| 00:37:16,900 | 00:37:17,700 | Robert | Okay, ne, passt. |
| 00:37:19,640 | 00:37:24,060 | Robert | Also wenn der Stromverbrauch, das war mir nicht bewusst, dass der Stromverbrauch so unkritisch ist für uns. |
| 00:37:25,480 | 00:37:30,160 | Robert | Das würde ja heißen, dass wir unter Umständen dann das Modem gar nicht runterfahren müssen, oder? |
| 00:37:30,170 | 00:37:35,080 | Robert | Weil das ist ja das Zeitintensive, in dem das Modem runter und hoch gefahren wird. |
| 00:37:35,340 | 00:37:40,120 | Marcel | Also wenn keine Buchung da ist, würde ich das Modem runterfahren, weil es einfach sinnlos ist, das laufen zu lassen. |
| 00:37:41,020 | 00:37:42,940 | Marcel | Warum soll ich das Modem laufen lassen |
| 00:37:42,960 | 00:37:43,920 | Marcel | und mit dem Netz verbunden sein? |
| 00:37:44,680 | 00:37:46,580 | Robert | Ja, weil wir schneller sind. |
| 00:37:47,100 | 00:37:48,320 | Robert | Wenn ich die Karte davor halte, |
| 00:37:48,800 | 00:37:49,740 | Robert | bin ich viel schneller da. |
| 00:37:50,220 | 00:37:52,300 | Robert | Also wenn es zum Stromverbrauch herwurscht ist, |
| 00:37:53,080 | 00:37:53,760 | Robert | das ist die Frage. |
| 00:37:54,220 | 00:37:55,260 | Robert | Okay, dann lass uns das mal |
| 00:37:57,420 | 00:37:59,060 | Marcel | als To-Do einfach festhalten, |
| 00:37:59,400 | 00:38:00,660 | Marcel | dass wir da den Finn |
| 00:38:00,780 | 00:38:01,360 | Marcel | nochmal darum bitten, |
| 00:38:02,480 | 00:38:04,840 | Marcel | dass er nochmal prüft, |
| 00:38:05,500 | 00:38:06,880 | Marcel | was ist denn der Stromverbrauch |
| 00:38:07,040 | 00:38:08,400 | Marcel | und auch der Stromverbrauchunterschied |
| 00:38:08,420 | 00:38:13,500 | Marcel | zwischen "Modem ist aktiv mit dem Internet verbunden", damit ich schnell eine Abfrage stellen kann, |
| 00:38:14,060 | 00:38:17,660 | Marcel | "Modem ist an und nicht mit dem Internet verbunden" und "Modem ist aus". |
| 00:38:18,070 | 00:38:20,620 | Marcel | Weil unter Umständen war es bei mir die Stromverbrauchung 40 mA, |
| 00:38:20,860 | 00:38:24,300 | Marcel | Modem ist zwar mit Strom versorgt, hat aber keine aktive Internetverbindung. |
| 00:38:24,830 | 00:38:25,840 | Marcel | Das kann ich dir jetzt nicht mehr sagen. |
| 00:38:28,100 | 00:38:30,540 | Robert | Das wäre super interessant. Und was sind die Zeiten? |
| 00:38:31,060 | 00:38:38,300 | Robert | Also was habe ich für Modi, wie viel Schwenk brauche ich und wie lange brauche ich um hochzukommen aus dem Modus. |
| 00:38:38,600 | 00:38:45,740 | Marcel | Ok, ja. Gut, dann machen wir dann nachher ein Tutu draus und schicken das dem Fynn, ob er das machen kann. |
| 00:38:46,980 | 00:38:48,960 | Robert | Gut. Perfekt. Zurück. |
| 00:38:51,300 | 00:38:55,380 | Robert | Wir haben jetzt drei Lösungsansätze skizziert. |
| 00:38:59,320 | 00:39:03,720 | Robert | Aber genau, die ist gut auf jeden Fall mit drei, was da rauskommt. |
| 00:39:04,500 | 00:39:10,620 | Marcel | Die Grundänderung, die wir jetzt gemacht haben, ist, dass wir gesagt haben, wir haben sowohl beim Use Case 1 Öffnen, |
| 00:39:11,020 | 00:39:17,780 | Marcel | beim Use Case 2 schließen, den Fall mit, ich ändere zwischen letztes Termin abfragen |
| 00:39:18,020 | 00:39:21,640 | Marcel | und jetzt, wenn ich an dem Auto stehe, etwas an den Buchungen, |
| 00:39:23,240 | 00:39:25,880 | Marcel | sei es der Fall 1, ich habe die Buchung storniert, |
| 00:39:26,020 | 00:39:28,800 | Marcel | du machst genau zeitgleich eine Buchung rein, kommst mit deiner Karte hin, |
| 00:39:29,680 | 00:39:32,560 | Marcel | dann muss der checken, es gibt eine Änderung in der Buchung. |
| 00:39:33,360 | 00:39:37,160 | Marcel | Deswegen haben wir gesagt, er muss vor einem anstehenden Termin |
| 00:39:37,400 | 00:39:39,940 | Marcel | und bis das Auto aufgesperrt wird, auf jeden Fall mal regelmäßig abfragen, |
| 00:39:40,040 | 00:39:43,500 | Marcel | gibt es eine Änderung, damit ein gelöschter Termin nicht trotzdem das Auto aufsperren kann. |
| 00:39:45,640 | 00:39:52,240 | Marcel | Und er muss, wenn jemand an einem Auto steht, der abgelehnt wird, weil er keine Reservierung hat, |
| 00:39:52,400 | 00:39:54,580 | Marcel | dann muss er so und so einfach mal die Termine aktualisieren. |
| 00:39:54,740 | 00:39:57,460 | Marcel | Beziehungsweise auch da wieder schneller und einfacher. |
| 00:39:58,680 | 00:40:00,460 | Marcel | Er fragt erstmal an, gibt es Terminänderungen? |
| 00:40:01,060 | 00:40:02,860 | Marcel | Wenn nein, muss er auch keine Termine runterladen. |
| 00:40:07,280 | 00:40:08,980 | Marcel | Weil wenn es keine Änderungen gibt, braucht er nichts runterladen. |
| 00:40:09,080 | 00:40:13,860 | Marcel | Wenn es eine Änderung gibt, dann fragt der nächsten Request mit einem zweiten Service an, |
| 00:40:14,630 | 00:40:16,040 | Marcel | gib mir mal die Termine und lädt die runter. |
| 00:40:17,819 | 00:40:21,160 | Marcel | Nur weil jemand eine Karte drauflegt, die abgelehnt wird, heißt das ja nicht, dass es auch gebucht hat. |
| 00:40:26,800 | 00:40:27,140 | Robert | Ja, genau. |
| 00:40:27,190 | 00:40:34,460 | Robert | Das mit dem "gibt es eine Änderung" ist eigentlich generell elegant, weil es damit, also generell, |
| 00:40:34,520 | 00:40:38,960 | Robert | unabhängig von wem Szenario sagen kannst, muss ich aktualisieren? |
| 00:40:40,130 | 00:40:44,020 | Robert | Im Endeffekt ist dieser Art Push ein niederschwelliger Punkt. |
| 00:40:45,220 | 00:40:49,920 | Robert | Zwischen vollwertigen Push und vollwertigen Pull. |
| 00:40:51,860 | 00:40:53,180 | Marcel | Ja, genau. |
| 00:40:53,180 | 00:40:59,180 | Marcel | Das ist ein Statusupdate, das halt schlank ist und das man eben aufgrund dessen, dass man |
| 00:40:58,980 | 00:41:02,780 | Marcel | keine Nutzdaten verträgt, nicht verschlüsselt, also rein mit HTTP machen kann. |
| 00:41:03,880 | 00:41:04,740 | Marcel | Ähm, genau. |
| 00:41:05,720 | 00:41:07,040 | Speaker 3 | Ja, cool. |
| 00:41:07,240 | 00:41:07,360 | Marcel | Gut. |
| 00:41:08,480 | 00:41:10,300 | Marcel | Dann haben wir aufgesperrt, wir haben zugesperrt. |
| 00:41:14,320 | 00:41:15,120 | Marcel | Ähm, jetzt... |
| 00:41:16,560 | 00:41:19,560 | Marcel | Hast du gemeint, wir machen kurz einen Abstecher, würde an der Stelle, glaube ich, passend |
| 00:41:19,620 | 00:41:24,600 | Marcel | in das Thema, wie gehen wir damit um, wenn ich zu früh ausleihen, geht nicht. |
| 00:41:25,460 | 00:41:29,560 | Marcel | blockiert, also da können wir eine Kulanzzeit oder müssen wir eine Kulanzzeit einbauen von wegen mir 5 Minuten, |
| 00:41:30,300 | 00:41:32,520 | Marcel | dass man schon 5 Minuten früher aufsperren kann vor dem Termin. |
| 00:41:34,160 | 00:41:38,820 | Marcel | Und das gleiche auch nach hinten, dass man auch 5 Minuten nach dem Termin auch noch 5 Grad sein lässt. |
| 00:41:40,360 | 00:41:41,900 | Robert | Also zusperren geht ja sowieso immer. |
| 00:41:42,300 | 00:41:43,740 | Marcel | Ja, ja, genau, richtig. Zusperren geht immer. |
| 00:41:44,340 | 00:41:52,080 | Marcel | Jetzt ist aber die Frage, was passiert, wenn ich ein Auto zusperre, nachdem der Termin eigentlich schon beendet ist? |
| 00:41:53,280 | 00:41:55,400 | Robert | Also das ist generell die Frage, was... |
| 00:41:55,980 | 00:41:57,420 | Robert | Ich würde es als Überziehung behandeln. |
| 00:41:58,140 | 00:42:00,480 | Robert | Und generell ist die Frage, was passiert bei Überziehen? |
| 00:42:03,340 | 00:42:05,180 | Marcel | Naja, ich bin jetzt ganz pragmatisch. |
| 00:42:06,000 | 00:42:07,540 | Marcel | Ich als ZK, ist mir scheißegal. |
| 00:42:11,660 | 00:42:12,360 | Marcel | Lass dir was einfallen. |
| 00:42:12,400 | 00:42:15,600 | Marcel | Du kriegst von mir als ZK nur die Info, jetzt ist zugesperrt mit der Karte. |
| 00:42:16,700 | 00:42:16,900 | Marcel | Bitte. |
| 00:42:18,560 | 00:42:22,579 | Robert | Ja, aber ich als Sharepad muss... |
| 00:42:23,940 | 00:42:24,820 | Robert | Perspektivische Abrechnung. |
| 00:42:25,350 | 00:42:25,480 | Robert | Genau. |
| 00:42:26,660 | 00:42:28,280 | Robert | Das heißt, ich muss nur um das Thema |
| 00:42:29,820 | 00:42:30,600 | Robert | Abrechnung, das ist |
| 00:42:30,700 | 00:42:32,460 | Robert | relativ klar, weil ich verlange |
| 00:42:32,580 | 00:42:34,440 | Robert | es halt einfach. Aber ich würde |
| 00:42:34,500 | 00:42:35,300 | Robert | einen Schritt weiter gehen. |
| 00:42:37,140 | 00:42:38,400 | Robert | Perspektivisch soll ich Sharepad |
| 00:42:38,760 | 00:42:40,320 | Robert | auch Verwarnungen |
| 00:42:40,560 | 00:42:41,900 | Robert | aussprechen, wenn ich überziehe. |
| 00:42:42,720 | 00:42:44,500 | Marcel | Aber jetzt ganz kurz, rede ich gerne |
| 00:42:44,530 | 00:42:46,180 | Marcel | mit dir drüber. Wenn wir jetzt den Fokus |
| 00:42:46,380 | 00:42:48,420 | Marcel | behalten wollen auf Use Cases |
| 00:42:48,600 | 00:42:50,500 | Marcel | und Schnittstelle zwischen ZK und Sharepad, |
| 00:42:51,640 | 00:42:54,700 | Marcel | ist die Schnittstelle zwischen ZK und Shepard an der Stelle einfach. |
| 00:42:55,380 | 00:42:56,840 | Marcel | Nämlich, ich sag, |
| 00:42:57,760 | 00:43:00,560 | Marcel | die ZK sagt dem Shepard, Auto wurde zugesperrt. |
| 00:43:00,900 | 00:43:01,720 | Marcel | Am, um, von. |
| 00:43:03,500 | 00:43:04,460 | Marcel | Und was du daraus machst, |
| 00:43:05,600 | 00:43:08,120 | Marcel | ob du eine Buchung verlängerst, ob du sagst, Moment, |
| 00:43:09,380 | 00:43:10,660 | Marcel | also das würde ich tatsächlich auch eben, |
| 00:43:10,800 | 00:43:12,800 | Marcel | dieses Zusperren-Information würde ich, |
| 00:43:13,360 | 00:43:16,620 | Marcel | das ist aber wichtig, deswegen ist es schon wichtig bei der Schnittstelle, |
| 00:43:17,260 | 00:43:20,940 | Marcel | Das Absperren würde ich als Live-Message machen. |
| 00:43:21,740 | 00:43:24,220 | Marcel | Das heißt, sobald der absperrt, schicke ich gleich eine Nachricht los. |
| 00:43:24,220 | 00:43:27,020 | Marcel | Es ist nicht ein Protokolleintrag, der am Abend übertragen wird, |
| 00:43:27,140 | 00:43:28,680 | Marcel | sondern es wird live an dich geschickt. |
| 00:43:29,240 | 00:43:32,180 | Marcel | Jetzt wurde das Auto von der Karte zugesperrt. |
| 00:43:36,220 | 00:43:37,040 | Marcel | Vielleicht kann man... |
| 00:43:37,040 | 00:43:43,120 | Robert | Das ist der Vorteil, dass ich die volle Kontrolle behalte, wie ich darauf reagiere. |
| 00:43:43,380 | 00:43:46,340 | Marcel | Genau. Und dann kannst du auch eine einstellbare Sache auf Dauer machen. |
| 00:43:46,520 | 00:43:48,580 | Marcel | kannst sagen, beim Verein A, dem ist es total wurscht, |
| 00:43:49,400 | 00:43:50,680 | Marcel | dann wird einfach die Zeit nachgebucht |
| 00:43:50,680 | 00:43:51,920 | Marcel | und fertig, beim Verein B, |
| 00:43:52,440 | 00:43:54,360 | Marcel | also perspektivisch gesehen, oder du machst |
| 00:43:54,600 | 00:43:56,300 | Marcel | zum Start natürlich eine einheitliche Regel und sagst, |
| 00:43:56,760 | 00:43:58,800 | Marcel | wenn du fünf Minuten vor Terminende |
| 00:43:58,980 | 00:44:01,040 | Marcel | oder beim Terminende noch kein Absperrensignal |
| 00:44:01,120 | 00:44:02,420 | Marcel | bekommen hast, dann schickst du eine SMS |
| 00:44:02,520 | 00:44:04,600 | Marcel | an die Handynummer und sagst, Achtung, |
| 00:44:04,600 | 00:44:06,780 | Marcel | lieber Benutzer, deine Buchungszeit endet, |
| 00:44:06,840 | 00:44:08,340 | Marcel | hast du dein Auto vergessen abzusperren |
| 00:44:08,580 | 00:44:10,080 | Marcel | oder willst du vielleicht deine Buchung verlängern? |
| 00:44:13,340 | 00:44:14,740 | Marcel | Was auch immer, das kann man sich im Prozess |
| 00:44:14,940 | 00:44:16,480 | Marcel | überlegen, aber aus ZK-Sicht |
| 00:44:16,500 | 00:44:18,960 | Marcel | ist wieder Logik wenig im Frontend, |
| 00:44:19,640 | 00:44:20,440 | Marcel | also in meiner ZK. |
| 00:44:20,990 | 00:44:21,940 | Marcel | Das heißt, ich gebe dir nur |
| 00:44:22,290 | 00:44:23,520 | Marcel | ein Live-Update, schickt dir, |
| 00:44:23,590 | 00:44:25,280 | Marcel | baue eine Modemverbindung auf, schickt dir eine Nachricht, |
| 00:44:25,880 | 00:44:29,660 | Marcel | REST-RP, bitteschön, hier, ZK, XYZ, |
| 00:44:30,120 | 00:44:32,200 | Marcel | hat mit Karten Nummer so und so zugesperrt. |
| 00:44:33,580 | 00:44:34,900 | Marcel | Perspektivisch könnte man noch überlegen, |
| 00:44:34,930 | 00:44:36,380 | Marcel | ob man GPS-Koordinaten mit reinhaut. |
| 00:44:38,940 | 00:44:40,040 | Marcel | Aber das ist dann wieder schwierig, |
| 00:44:40,150 | 00:44:42,460 | Marcel | weil das hat ja wieder das Thema Überwachung, |
| 00:44:42,980 | 00:44:44,460 | Marcel | irgendwas, unnötige Daten, |
| 00:44:45,800 | 00:44:46,320 | Marcel | Nein, ich verlinke mich nicht. |
| 00:44:46,760 | 00:44:46,900 | Robert | Was? |
| 00:44:47,480 | 00:44:52,580 | Robert | Also, wenn das Auto am Stellplatz steht, dann ist die Koordinate selbstverständlich. |
| 00:44:53,080 | 00:44:58,980 | Robert | Wenn das Auto nicht am Stellplatz steht, dann bin ich der Meinung, dass der Verein das wissen darf, wo das Auto übergeben wurde. |
| 00:44:59,380 | 00:45:03,720 | Marcel | Hm, außer, ich mach das so, wie wir es auch schon mal gemacht haben. |
| 00:45:04,340 | 00:45:08,120 | Marcel | Ich bleibe im Auto sitzen, während meine Frau mit dem Schlüssel in den Laden geht. |
| 00:45:08,940 | 00:45:11,040 | Marcel | Und dann denke ich mir, ja, eigentlich ist es mir jetzt auch fahrt im Auto. |
| 00:45:11,110 | 00:45:13,380 | Marcel | Ich stecke aus, gehe zu meiner Frau und sperre das Auto mit der Karte ab. |
| 00:45:13,960 | 00:45:17,220 | Marcel | Weil, den Schlüssel hatte ja meine Frau dabei, weil ich im Auto bleiben wollte und noch am Handy was machen. |
| 00:45:19,120 | 00:45:20,300 | Marcel | Und sie hätte den Schlüssel einfach eingesteckt. |
| 00:45:20,300 | 00:45:22,740 | Robert | Ich möchte die Karte nicht her, um final zuzusperren. |
| 00:45:22,780 | 00:45:23,440 | Marcel | Nein, aber ich sperre zu. |
| 00:45:23,990 | 00:45:25,520 | Marcel | Ich sperre mitten in meiner Buchung zu. |
| 00:45:25,670 | 00:45:31,960 | Marcel | Und woher willst du wissen, ist das eine finale Absperrung oder ist das eine Zwischenabschmerung mit der Karte? |
| 00:45:32,120 | 00:45:37,340 | Robert | Das heißt, du protokollierst jedes Absperren an mich. |
| 00:45:38,120 | 00:45:38,980 | Marcel | Genau, ja, muss ich ja. |
| 00:45:39,200 | 00:45:41,080 | Marcel | Also jedes Absperren mit Karten. |
| 00:45:41,720 | 00:45:48,260 | Marcel | Und im Normalfall macht keiner das, oder sehr sehr selten, dass er mit der Karte absperrt, obwohl er den Schlüssel während der Buchung gehandelt hat. |
| 00:45:48,540 | 00:45:50,440 | Marcel | Weil er das Speise mit dem Schlüssel abgeht, ja viel einfacher und schneller. |
| 00:45:52,000 | 00:46:00,360 | Marcel | Aber es gibt Fälle, explizit ich habe das schon gemacht, dass ich mit der Karte das Auto während einer laufenden Buchung aufgesperrt habe, oder auch mit der Karte zugesperrt habe. |
| 00:46:00,460 | 00:46:04,540 | Marcel | Weil es einfach gerade praktisch war. Ich hatte den Schlüssel nicht, wollte aber schnell was aus dem Auto holen. |
| 00:46:04,560 | 00:46:10,260 | Marcel | Ach Mist, der liegt im Zelt, egal, schnell Karte raus, die habe ich dabei, aufsperren, rausholen, wieder absperren. |
| 00:46:11,360 | 00:46:18,360 | Marcel | Deswegen haben wir auch gesagt, ist ein Auf- und Absperren mit der Karte kein automatisches Buchungsende. |
| 00:46:18,360 | 00:46:26,480 | Marcel | Es ist ein Hinweis, wenn ich nach dem Ende der Buchung noch nicht abgesperrt habe, dann ist es ein Hinweis auf, hey, du hast überzogen. |
| 00:46:26,480 | 00:46:31,620 | Marcel | Aber wenn ich während einer Buchung absperre, ist es kein Indiz für die Buchung, die Fahrt ist zu Ende. |
| 00:46:31,620 | 00:46:38,860 | Marcel | Weil ich kann nämlich auch gemeinerweise mit der Karte absperren, danach mit dem Funkschlüssel aufsperren, weiterfahren und dann wieder mit der Karte absperren. |
| 00:46:39,920 | 00:46:40,640 | Marcel | Oder umgekehrt. |
| 00:46:41,560 | 00:46:43,820 | Marcel | Deswegen habe ich ja gesagt, ist dieses Kilometer erfassen, |
| 00:46:44,500 | 00:46:46,300 | Marcel | andere Baustelle jetzt, aber kurz als Einwurf, |
| 00:46:46,780 | 00:46:50,860 | Marcel | das Kilometer erfassen für mich mit dem Zu- und Absperren komplett entkoppelt. |
| 00:46:50,860 | 00:46:57,420 | Marcel | Weil ich sage, ich track ja mit, wo bist das Auto, um die Fahrtstrecken mit zu tracken. |
| 00:46:57,420 | 00:47:04,020 | Marcel | Und ich gebe dir immer Blöcke von der ZK ans Shepard, |
| 00:47:04,020 | 00:47:09,040 | Marcel | indem ich dir sage, von Uhrzeit am Datum bis Uhrzeit am Datum waren so und so viele Kilometer. |
| 00:47:09,660 | 00:47:12,940 | Marcel | Von Uhrzeit und Datum bis Uhrzeit und Datum waren so und so viele Kilometer. |
| 00:47:13,120 | 00:47:15,460 | Marcel | Und DU musstest dann matchen mit irgendwelchen Buchungen bei Dir. |
| 00:47:19,760 | 00:47:22,000 | Robert | Ja, ich bin mir nicht sicher, ob es nicht... |
| 00:47:24,840 | 00:47:26,440 | Robert | ob mir sich dann gefallen wird. |
| 00:47:27,620 | 00:47:31,820 | Robert | Also man könnte ja hergehend sagen, wenn Du mit der Karte zusperrst, ist die Buchung vorbei. |
| 00:47:34,100 | 00:47:34,300 | Speaker 3 | Hm. |
| 00:47:36,940 | 00:47:39,660 | Robert | Das Problem ist, wir können es nicht erzwingen bei den Benutzer. |
| 00:47:39,660 | 00:47:41,960 | Robert | Wie du sagst, der Benutzer kann hergehen, |
| 00:47:42,800 | 00:47:44,680 | Robert | kann den Schlüssel aus dem Auto rausnehmen, |
| 00:47:45,240 | 00:47:45,840 | Robert | sperrt zu |
| 00:47:46,640 | 00:47:49,100 | Robert | und selbst wenn wir das dann verhindern, dass du mehr aufsperrst, |
| 00:47:49,640 | 00:47:50,480 | Marcel | können wir nicht verhindern. |
| 00:47:51,320 | 00:47:53,500 | Marcel | Du kannst mit dem Schlüssel immer aufsperren, das können wir nicht verhindern. |
| 00:47:55,320 | 00:47:55,540 | Robert | Genau. |
| 00:47:56,880 | 00:48:00,360 | Marcel | Insofern ist das Auf- und Absperren mit der Karte |
| 00:48:00,360 | 00:48:02,480 | Marcel | ein Indiz, |
| 00:48:02,480 | 00:48:05,060 | Marcel | aber kein sicherer Hinweis. |
| 00:48:10,559 | 00:48:12,740 | Marcel | Also, die Boswilligkeit, |
| 00:48:13,780 | 00:48:14,660 | Marcel | dass ich sage, ich |
| 00:48:15,560 | 00:48:17,000 | Marcel | habe eine Buchung gemacht, eine Viertelstunde, |
| 00:48:17,220 | 00:48:18,080 | Marcel | sperre das Auto auf, |
| 00:48:19,580 | 00:48:20,920 | Marcel | sperre es gleich wieder mit der Karte ab, |
| 00:48:20,950 | 00:48:22,400 | Marcel | habe mir aber in der Zwischenzeit einen Schlüssel rausgeholt, |
| 00:48:22,460 | 00:48:23,920 | Marcel | sperre es dann mit dem Schlüssel auf und fahre eine Runde, |
| 00:48:25,140 | 00:48:26,080 | Marcel | die kann gegeben sein. |
| 00:48:26,680 | 00:48:28,600 | Marcel | Aber ganz ehrlich, da habe ich dann wieder den Vorteil, |
| 00:48:28,640 | 00:48:30,800 | Marcel | wenn ich den mit dem Kilometerprotokoll übermittle, |
| 00:48:31,400 | 00:48:33,100 | Marcel | von bis Strecke gefahren, |
| 00:48:34,000 | 00:48:37,840 | Marcel | dann ist auch relativ klar rauszukriegen, wer hat als letztes davor das Auto mit Karte aufgesperrt. |
| 00:48:37,980 | 00:48:38,960 | Marcel | Der wird es wohl gewesen sein. |
| 00:48:39,800 | 00:48:40,760 | Robert | Das weiß ich so auch. |
| 00:48:40,900 | 00:48:44,300 | Robert | Also wenn der aufgesperrt hat und dann noch das Auto weg. |
| 00:48:45,480 | 00:48:45,640 | Marcel | Genau. |
| 00:48:45,900 | 00:48:48,180 | Robert | Also der aufsperrt zu und dann noch das Auto weg. |
| 00:48:48,460 | 00:48:48,560 | Marcel | Ja. |
| 00:48:49,740 | 00:48:51,580 | Robert | Dann wäre ich auf jeden Fall mal bei dem nachgetragen. |
| 00:48:51,820 | 00:48:51,980 | Marcel | Genau. |
| 00:48:52,880 | 00:48:53,900 | Marcel | Der muss das letztes. |
| 00:48:54,280 | 00:48:54,540 | Marcel | Richtig. |
| 00:48:55,420 | 00:49:00,760 | Marcel | Deswegen, also ich bin der Meinung, man sollte das Auf- und Zusperrsignal als Indiz, |
| 00:49:00,840 | 00:49:05,900 | Marcel | aber es ist also das Aufsperren zum Startzeitpunkt eines Termins, |
| 00:49:07,960 | 00:49:12,160 | Marcel | wenn der Termin vorher beendet war und es wurde abgesperrt, ist das Aufsperren ein klarer Startpunkt. |
| 00:49:12,820 | 00:49:15,500 | Marcel | Das Absperren per Karte ist aber kein klarer Endpunkt. |
| 00:49:20,160 | 00:49:25,220 | Marcel | Es ist ganz oft ein Endpunkt, aber halt nicht zuverlässig, nicht sicher. |
| 00:49:26,340 | 00:49:29,880 | Robert | Es ist fast immer der Endpunkt sogar, aber es ist nicht sicher. |
| 00:49:30,480 | 00:49:32,560 | Marcel | in wahrscheinlich 98 oder 99, |
| 00:49:32,770 | 00:49:34,360 | Marcel | sogar vielleicht noch mehr als 99% der Fälle, |
| 00:49:34,420 | 00:49:35,860 | Marcel | ist das Absperren mit der Karte der Endpunkt. |
| 00:49:39,500 | 00:49:39,780 | Robert | Okay. |
| 00:49:43,220 | 00:49:43,620 | Marcel | Aber deswegen |
| 00:49:45,280 | 00:49:46,160 | Marcel | überziehen heißt, |
| 00:49:46,460 | 00:49:48,060 | Marcel | irgendwann wird mit der Karte abgesperrt. |
| 00:49:48,330 | 00:49:49,500 | Marcel | Und deswegen war ich bei dem Thema, |
| 00:49:50,140 | 00:49:52,240 | Marcel | darüber sind wir auf das Sidequest gekommen gerade, |
| 00:49:52,420 | 00:49:54,300 | Marcel | mit dem Auf- und Zusperren per Karte |
| 00:49:54,300 | 00:49:56,380 | Marcel | während der Buchung, ist die Frage, |
| 00:49:56,380 | 00:49:58,460 | Marcel | kann ich beim Absperren Gugens |
| 00:49:58,460 | 00:49:59,900 | Marcel | Gewissens die GPS-Koordinaten mitschicken? |
| 00:50:00,700 | 00:50:02,620 | Marcel | weil das wäre ja auch noch ein interessanter Punkt |
| 00:50:03,320 | 00:50:04,740 | Marcel | da ich aber auch während der Fahrt |
| 00:50:04,860 | 00:50:05,380 | Marcel | absperren kann |
| 00:50:07,500 | 00:50:08,680 | Marcel | ist dann wieder die Frage |
| 00:50:09,360 | 00:50:11,420 | Marcel | ob das okay ist |
| 00:50:11,520 | 00:50:13,100 | Marcel | dass das Sharepad weiß |
| 00:50:13,860 | 00:50:15,460 | Marcel | dass ich wie ich zum Puff gefahren bin |
| 00:50:15,560 | 00:50:17,500 | Marcel | den Schlüssel lieber in den Safe gelassen habe |
| 00:50:17,500 | 00:50:18,600 | Marcel | und mit Karte abgesperrt habe |
| 00:50:18,680 | 00:50:20,000 | Marcel | damit sie mir so einen Schlüssel nicht klauen |
| 00:50:20,040 | 00:50:22,540 | Marcel | von meinem schönen Lamborghini von den Autoteilern |
| 00:50:23,040 | 00:50:24,620 | Marcel | und dass dann der Autoteilerverein weiß |
| 00:50:25,140 | 00:50:27,560 | Marcel | oh Marcel, wieder gebucht und beim Puff geparkt |
| 00:50:29,520 | 00:50:29,920 | Marcel | wunderbar |
| 00:50:29,940 | 00:50:35,340 | Marcel | der Fall ist. Also gibt es vielleicht auch auf weiblicher Seite ähnlich prekäre Situationen? |
| 00:50:36,360 | 00:50:36,660 | Marcel | Aber... |
| 00:50:39,120 | 00:50:44,160 | Robert | Ja, also es ist tatsächlich... Die Frage ist, ob man den Fall wirklich berücksichtigen muss. |
| 00:50:44,600 | 00:50:51,220 | Robert | Dass man sagt, wenn du das wirklich machst, dass du den Schlüssel rausnimmst, oder generell, |
| 00:50:51,320 | 00:50:59,200 | Robert | dass du mit der Karte während der Fahrt zusperrst und dann wieder auf. Dass du dann die GPS-Koordinaten |
| 00:50:59,220 | 00:51:03,540 | Robert | nicht ein SharePay übermittelt. Noch dazu, wir haben ja noch nicht davon geredet, dass |
| 00:51:03,540 | 00:51:09,760 | Robert | wir die GPS-Koordinaten am Benutzer-Obsang oder sonst irgendjemandem-Obsang. Das erste Mal |
| 00:51:09,880 | 00:51:14,520 | Robert | reden wir nur über die Übermittlung. Was wir damit machen, das steht ja auf dem anderen |
| 00:51:14,760 | 00:51:22,120 | Robert | Stern. Denkbar wäre zum Beispiel, dass man sagt, wenn das Auto gestohlen wird, was bringen |
| 00:51:22,140 | 00:51:27,900 | Marcel | uns die Koordinaten eigentlich? Die Koordinaten würden eine Validierung ermöglichen, handelt |
| 00:51:27,920 | 00:51:29,480 | Marcel | sich um ein endgültiges Absperren, |
| 00:51:30,060 | 00:51:32,160 | Marcel | weil die GPS-Koordinaten in der Nähe |
| 00:51:32,270 | 00:51:33,900 | Marcel | oder auf dem Standort liegen, |
| 00:51:34,160 | 00:51:35,280 | Marcel | wo das Auto parken sollte. |
| 00:51:35,700 | 00:51:36,920 | Marcel | Die sind ja beim Auto hinterlegt. |
| 00:51:38,020 | 00:51:39,600 | Marcel | Und wenn die dort absperrt werden, |
| 00:51:39,860 | 00:51:41,840 | Marcel | sage ich, dann ist es sehr wahrscheinlich ein Ende der Buchung. |
| 00:51:41,940 | 00:51:43,840 | Marcel | Sind die GPS-Koordinaten aber bei Müller, |
| 00:51:44,160 | 00:51:45,840 | Marcel | also nicht in meinem lokalen Gebiet, |
| 00:51:46,900 | 00:51:48,480 | Marcel | so what, dann ist es definitiv kein Ende, |
| 00:51:48,530 | 00:51:49,900 | Marcel | weil wenn er das bei sich zu Hause absperrt, |
| 00:51:49,980 | 00:51:51,300 | Marcel | das Auto ist die Buchung nicht zu Ende damit. |
| 00:51:51,700 | 00:51:54,060 | Marcel | Dann überzieht er, auch wenn er mit Karte absperrt. |
| 00:51:55,300 | 00:51:56,700 | Marcel | Weil das die Rückgabe ist erst Erfolg, |
| 00:51:57,080 | 00:51:59,500 | Marcel | wenn das Auto wieder an seinem Stellplatz steht und dort abgeschlossen wird. |
| 00:51:59,900 | 00:52:01,600 | Robert | Aber auch nur wahrscheinlich. |
| 00:52:02,640 | 00:52:04,180 | Robert | Es gibt Fälle, also, |
| 00:52:04,680 | 00:52:05,880 | Robert | wie bei uns hat das zum Beispiel, |
| 00:52:06,340 | 00:52:07,420 | Robert | dass es irgendein Umzug oder so, |
| 00:52:07,510 | 00:52:09,520 | Robert | die Autos müssen umgepackt werden. |
| 00:52:11,020 | 00:52:13,200 | Robert | Und dann werden die woanders doch gestellt. |
| 00:52:14,460 | 00:52:14,660 | Robert | Also, |
| 00:52:15,420 | 00:52:17,000 | Robert | nur weil sie woanders stehen, |
| 00:52:17,220 | 00:52:18,280 | Robert | heißt es nicht sicher, |
| 00:52:20,440 | 00:52:23,600 | Robert | dass sie überziehen. |
| 00:52:23,980 | 00:52:24,100 | Marcel | Okay. |
| 00:52:24,540 | 00:52:26,760 | Marcel | Deswegen, lass uns dieses GPS-Koordinatending vergessen. |
| 00:52:27,060 | 00:52:27,840 | Marcel | streichen wir? |
| 00:52:29,100 | 00:52:30,900 | Robert | Nein, das würde ich damit nicht sagen. |
| 00:52:31,760 | 00:52:32,460 | Robert | Ich würde nur sagen, |
| 00:52:32,860 | 00:52:35,380 | Robert | eine sichere Bank ist es so oder so nicht, |
| 00:52:35,440 | 00:52:35,740 | Robert | aber |
| 00:52:38,740 | 00:52:40,920 | Robert | ich fände es trotzdem gut. |
| 00:52:42,020 | 00:52:43,380 | Robert | Also die Frage ist, |
| 00:52:43,670 | 00:52:44,960 | Robert | was würde ich damit machen? |
| 00:52:46,760 | 00:52:48,680 | Robert | Also wir sollten auf keinen Fall |
| 00:52:49,600 | 00:52:50,360 | Robert | gut sagen. |
| 00:52:50,520 | 00:52:51,560 | Marcel | Nein, das auch sowieso nicht. |
| 00:52:51,630 | 00:52:52,800 | Marcel | Ich würde sie auch gar nicht, |
| 00:52:53,340 | 00:52:54,280 | Marcel | also meine Idee wäre, |
| 00:52:54,500 | 00:52:55,300 | Marcel | ich würde sie nicht |
| 00:52:59,440 | 00:53:04,160 | Marcel | Speichern ist halt wieder schwierig. GPS-Koordinaten sind definitiv sensible Daten in meinen Augen. |
| 00:53:05,840 | 00:53:11,000 | Marcel | Und wenn man ihn jetzt nutzt, um zu prüfen, steht das Auto auf oder in der Nähe des Parkplatzes, |
| 00:53:12,480 | 00:53:18,780 | Marcel | dann kann man auch ein "steht am Autoparkplatz" oder "steht in der Region des Autoparkplatzes" |
| 00:53:18,840 | 00:53:24,460 | Marcel | ja/nein in die Datenbank speichern beim Absperren. Das ist absolut unverfänglich und voll okay, |
| 00:53:24,480 | 00:53:28,100 | Marcel | Damit kann ich nicht zurückziehen. Ich weiß nur, es wurde mit der Karte um die Uhr abgesperrt. |
| 00:53:28,490 | 00:53:30,120 | Marcel | Stand nicht am Parkplatz, stand am Parkplatz. |
| 00:53:31,720 | 00:53:31,900 | Marcel | Passt. |
| 00:53:34,819 | 00:53:39,440 | Marcel | Wenn ich aber die GPS-Koordinaten speichere, sind es wieder schützenswerte Daten in meinen Augen. |
| 00:53:42,260 | 00:53:45,180 | Marcel | Dann ist die Frage, das macht es für dich aufwändiger? |
| 00:53:45,480 | 00:53:47,380 | Robert | Ich würde es anders sehen. |
| 00:53:48,100 | 00:53:49,440 | Robert | Wenn ich sie speichere, |
| 00:53:50,920 | 00:53:51,880 | Robert | während der Buchung, |
| 00:53:52,360 | 00:53:53,840 | Robert | sobald die Buchung zu Ende ist, |
| 00:53:54,140 | 00:53:55,180 | Robert | bin ich der Meinung, |
| 00:53:55,420 | 00:53:56,440 | Robert | dass ich sie speichern darf. |
| 00:53:59,860 | 00:54:01,520 | Robert | Dann hast du das Auto |
| 00:54:01,960 | 00:54:03,000 | Robert | an einem Punkt abgestellt, |
| 00:54:03,020 | 00:54:04,660 | Robert | der nicht mehr dich persönlich betrifft. |
| 00:54:06,020 | 00:54:06,580 | Marcel | Da hast du recht. |
| 00:54:07,780 | 00:54:09,460 | Marcel | Jetzt kommen wir wieder zu dem spannenden Punkt. |
| 00:54:10,260 | 00:54:11,900 | Marcel | Die Buchung ist zu Ende, |
| 00:54:12,660 | 00:54:14,619 | Marcel | wenn es das letzte Absperren |
| 00:54:14,740 | 00:54:16,060 | Marcel | vor Reservierungsende war. |
| 00:54:17,260 | 00:54:17,580 | Robert | Genau. |
| 00:54:18,230 | 00:54:19,480 | Marcel | Also das heißt, ich sperre fünfmal |
| 00:54:19,640 | 00:54:21,880 | Marcel | ab, jedes Absperren |
| 00:54:22,320 | 00:54:23,700 | Marcel | schickt dir wieder neue Daten, du merkst |
| 00:54:23,760 | 00:54:25,640 | Marcel | bloß zu dieser Buchung, letzter Absperrvorgang |
| 00:54:25,720 | 00:54:26,820 | Marcel | waren die GPS-Koordinaten |
| 00:54:27,580 | 00:54:29,400 | Marcel | und ab dem Moment, wo ich das Auto, wo die |
| 00:54:29,540 | 00:54:31,120 | Marcel | Reservierung zu Ende ist, sagst du, okay, |
| 00:54:31,580 | 00:54:33,440 | Marcel | den letzten Wert in meinem Puffer |
| 00:54:33,940 | 00:54:35,800 | Marcel | für GPS-Koordinaten beim Absperren, |
| 00:54:36,000 | 00:54:37,520 | Marcel | den merke ich mir als |
| 00:54:38,059 | 00:54:38,420 | Marcel | Rückgabeort. |
| 00:54:39,590 | 00:54:39,680 | Robert | Genau. |
| 00:54:40,180 | 00:54:42,440 | Marcel | Das ist okay, das halte ich auch datentechnisch für sauber. |
| 00:54:43,520 | 00:54:46,420 | Robert | Also ich würde es im RAM dann vielleicht sogar vorhalten. |
| 00:54:48,640 | 00:54:57,480 | Robert | Was nicht ideal ist, boah, also das, okay, das sind technische Details, glaube ich. |
| 00:54:58,250 | 00:55:08,080 | Robert | Wie viel datenschutztechnisch relevant ist, dass man die GPS-Koordinaten nur nach der Reservierung dauerhaft speichern. |
| 00:55:08,420 | 00:55:11,460 | Robert | Was zwischen Absperren und Reservierungsende passiert. |
| 00:55:12,240 | 00:55:13,220 | Robert | Können wir das mal offen lassen? |
| 00:55:14,080 | 00:55:14,200 | Robert | Ja. |
| 00:55:15,660 | 00:55:16,340 | Robert | Können wir offen lassen. |
| 00:55:16,420 | 00:55:21,600 | Robert | Also, für mich wäre es lieber, ehrlich gesagt, wenn ich es in der Datenbank speichern dürfte, |
| 00:55:22,220 | 00:55:24,360 | Robert | aber natürlich dann löscht aus der Datenbank. |
| 00:55:27,160 | 00:55:30,220 | Marcel | Das ist ein Thema, das musst du datentechnisch durchleuchten. |
| 00:55:30,220 | 00:55:32,080 | Marcel | Da bin ich auch nicht gut drin bei solchen Themen. |
| 00:55:33,400 | 00:55:33,560 | Robert | Mhm. |
| 00:55:34,260 | 00:55:37,820 | Marcel | Da ist tatsächlich da aber vielleicht eher eine Session mit der Tabea-Rissicht besser. |
| 00:55:38,520 | 00:55:43,180 | Marcel | Weil die Tabea ja mit dem Jörg zusammen die Datenschutzvereinbarung, die neue Wumpf hat, entwickelt hat. |
| 00:55:44,380 | 00:55:46,020 | Robert | Ja, da weiß ich, ob das rauskommt. |
| 00:55:47,960 | 00:55:49,000 | Robert | Dann würde ich doch leuchten. |
| 00:55:50,540 | 00:55:51,880 | Marcel | Dann entscheide es für dich. |
| 00:55:52,410 | 00:55:53,280 | Marcel | Du bist der Betreiber. |
| 00:55:54,020 | 00:56:00,080 | Marcel | Und im Zweifel musst du einer Benutzeranfrage von irgendjemandem Rede und Antwort stehen und das argumentieren. |
| 00:56:00,580 | 00:56:03,040 | Marcel | Die Wahrscheinlichkeit, dass das passiert, halte ich für äußerst gering. |
| 00:56:05,020 | 00:56:06,580 | Marcel | Aber du solltest darauf vorbereitet sein. |
| 00:56:08,440 | 00:56:09,920 | Marcel | was du antwortest und warum. |
| 00:56:22,040 | 00:56:22,560 | Marcel | Beziehungsweise |
| 00:56:23,880 | 00:56:24,140 | Marcel | jetzt |
| 00:56:25,100 | 00:56:26,920 | Marcel | ich weiß nicht, ob ich da zu spitz finde ich werde. |
| 00:56:27,900 | 00:56:28,740 | Marcel | Es ist ja so, dass |
| 00:56:30,380 | 00:56:32,180 | Marcel | muss denn jeder, also wenn ich |
| 00:56:32,300 | 00:56:33,300 | Marcel | bei Sharepad buche, |
| 00:56:35,040 | 00:56:36,040 | Marcel | ist es nicht ein Irrsinn, |
| 00:56:36,100 | 00:56:42,720 | Marcel | dass jeder Verein die Nutzung, also was speichert du für Daten, was machst du für sich auskastet. |
| 00:56:42,720 | 00:56:47,920 | Marcel | Im Endeffekt muss doch nur jeder Verein in seiner Datenschutzverordnung drinstehen haben, |
| 00:56:48,420 | 00:56:54,240 | Marcel | dass die Daten, die zur Abrechnung, zur Fahrt und zur Reservierung, Buchung etc. benötigt werden, |
| 00:56:54,340 | 00:56:55,260 | Marcel | in Sharepad gespeichert sind. |
| 00:56:56,880 | 00:57:04,540 | Marcel | Und an dich verweisen und du sagst dann im Zweifel folgende Daten werden wie gespeichert. |
| 00:57:05,160 | 00:57:05,740 | Robert | Genau. |
| 00:57:06,160 | 00:57:08,420 | Marcel | Und damit liegt es ja oft in deiner Hand. |
| 00:57:09,070 | 00:57:13,020 | Marcel | Und dann ist es so, wo man sagt, okay, ich halte es für legitim, |
| 00:57:13,740 | 00:57:15,780 | Marcel | das kann man aber auch in der größeren Runde nochmal diskutieren, |
| 00:57:16,360 | 00:57:21,780 | Marcel | ich halte es für legitim, dass man den GPS-Koordinaten beim Abspeichern speichert, |
| 00:57:22,500 | 00:57:26,020 | Marcel | und zwar solange eine Reservierung gilt, immer die letzten Absperrkoordinaten, |
| 00:57:26,090 | 00:57:27,460 | Marcel | die bleiben in der Datenbank gespeichert. |
| 00:57:27,620 | 00:57:31,500 | Marcel | Das heißt, für einen Übergangszeitraum anfangen, wenn ich am Anfang der Buchung |
| 00:57:33,100 | 00:57:36,420 | Marcel | irgendwo wo ich es nicht will oder generell irgendwo wo nicht der |
| 00:57:36,480 | 00:57:39,960 | Marcel | Abstellort ist absperre, dann sind die Koordinaten in deiner Datenbank gespeichert |
| 00:57:40,500 | 00:57:42,600 | Marcel | und die sind so lange drin bis ich es wieder zurück gebe. |
| 00:57:44,260 | 00:57:46,200 | Marcel | Angenommen mir passiert was, ich habe einen Unfall, |
| 00:57:47,650 | 00:57:50,600 | Marcel | whatever, dann ist es auch nicht blöd zu wissen wo das Auto steht. |
| 00:57:53,840 | 00:57:55,220 | Marcel | Also bald ich nochmal... |
| 00:57:55,620 | 00:57:58,960 | Robert | Wir reden ja nur von diesem Sonderfall, wo wir vorher schon gesagt haben, |
| 00:57:59,420 | 00:58:04,560 | Robert | dass du während der Buchung zwischen- und dazusperrst. |
| 00:58:04,560 | 00:58:09,060 | Robert | Weil wenn du final zusperrst, aber wenn es viel früher ist wie am Ende, |
| 00:58:09,060 | 00:58:11,300 | Robert | ist es trotzdem die finale Koordinate. |
| 00:58:11,300 | 00:58:14,140 | Robert | Genau, und dann ist es legitim. |
| 00:58:14,140 | 00:58:23,620 | Robert | Von einem 1%-Fall sprechen wir, und dort sehen wir die Daten für einen Zeitraum von zusperren bis erneut zusperren, |
| 00:58:26,240 | 00:58:32,700 | Robert | Ungerechtfertigter Weise speichern, aber auch nur so, dass in der Datenbank steht, ohne dass der Benutzer abrufen kann. |
| 00:58:33,240 | 00:58:35,300 | Marcel | Weder Benutzer noch Admin können sie abrufen. |
| 00:58:36,240 | 00:58:38,680 | Robert | Genau, keiner kann sie abrufen. Niemand kann sich abrufen. |
| 00:58:40,980 | 00:58:42,880 | Marcel | Gut, gecathlet, wir machen das so. |
| 00:58:43,480 | 00:58:48,540 | Marcel | Mit dem Absperren ermitteln wir GPS-Koordinaten und übermitteln die GPS-Koordinaten an dich. |
| 00:58:49,040 | 00:58:56,300 | Marcel | Das heißt, du kriegst, also sprich der Service, der heißt Absperrdaten übermitteln an Sharepad |
| 00:58:56,840 | 00:59:03,680 | Marcel | und da kriegst du mit, welche Karten-ID hat, welche Uhrzeit, an welchem Tag, das Auto zugesperrt, an welchen GPS-Koordinaten. |
| 00:59:04,640 | 00:59:12,620 | Robert | Das ist super, weil dann kann ich tatsächlich am meisten Informationen, |
| 00:59:12,920 | 00:59:16,660 | Robert | Aber bei meisten Infos, also so wie das Protokoll jetzt definiert ist, |
| 00:59:17,420 | 00:59:20,000 | Robert | und kann am meisten Logik in Sharepad verlagern, |
| 00:59:20,160 | 00:59:22,120 | Robert | und Frontend bleibt einfach dumm. |
| 00:59:24,440 | 00:59:27,900 | Robert | Wie wir das dann handhaben, können wir dann später sich überlegen. |
| 00:59:28,320 | 00:59:29,940 | Marcel | Genau, das müssen wir dann nochmal definieren. |
| 00:59:31,720 | 00:59:34,340 | Robert | Aber es hat nichts mit ZK zu tun. |
| 00:59:34,460 | 00:59:35,920 | Robert | Also das ZK braucht man dann involvieren. |
| 00:59:36,600 | 00:59:36,720 | Marcel | Genau. |
| 00:59:37,680 | 00:59:40,520 | Marcel | Wie gesagt, wir müssen auch immer mit Klaus noch mal reden, |
| 00:59:40,540 | 00:59:42,580 | Marcel | der war, das habe ich das letzte Mal schon erzählt, |
| 00:59:42,760 | 00:59:44,660 | Marcel | dass wir da ein bisschen aneinander |
| 00:59:44,780 | 00:59:45,180 | Marcel | gerumpelt sind. |
| 00:59:46,860 | 00:59:48,440 | Robert | Aber wegen was für ein Thema eigentlich genau, |
| 00:59:48,600 | 00:59:49,560 | Marcel | das habe ich mir nicht ganz verstanden? |
| 00:59:53,160 | 00:59:54,680 | Marcel | Ich kann es ja auch nicht mehr im Genauen sagen, |
| 00:59:54,800 | 00:59:56,540 | Marcel | im Endeffekt für mich das Fazit war, dass er halt |
| 00:59:56,660 | 00:59:58,640 | Marcel | ständig der Meinung ist, dass wir |
| 00:59:59,480 | 01:00:00,560 | Marcel | Entscheidungen treffen und die so |
| 01:00:00,620 | 01:00:02,540 | Marcel | nicht gehen, weil wir alle damit leben müssen und |
| 01:00:02,560 | 01:00:04,500 | Marcel | ganz viele unter der Entscheidung sagt, du Klaus, wir sind |
| 01:00:04,600 | 01:00:06,220 | Marcel | beide nicht dumm, wir haben beide was Ding und vielleicht |
| 01:00:06,480 | 01:00:07,740 | Marcel | vergisst man was, dann muss man es halt nachziehen. |
| 01:00:08,540 | 01:00:16,580 | Marcel | Aber dieses ständig unter Generalverdacht stellen, dass wir nicht im Sinne des Vereins handeln und Dinge vergessen oder extra anders machen, nervt mich. |
| 01:00:17,230 | 01:00:26,900 | Marcel | Und viele Dinge, die wir besprechen, merkt man auch dann nach 10 Minuten mit dir reden, dass du gar nicht genau verstanden hast, was wir eigentlich reden, weil es sehr technisch ist. |
| 01:00:28,280 | 01:00:33,620 | Marcel | Und deswegen ist es einfach anstrengend, so war grob die Zusammenfassung des Gesprächs. |
| 01:00:35,160 | 01:00:37,040 | Robert | Ja gut, wobei wir keine Entscheidungen... |
| 01:00:38,720 | 01:00:40,280 | Marcel | Doch, natürlich haben wir Entscheidungen getroffen. |
| 01:00:40,360 | 01:00:43,260 | Marcel | Wir haben eine Entscheidung getroffen, zum Beispiel die Mitgliedsnummer in die Tonne zu treten. |
| 01:00:44,020 | 01:00:44,860 | Marcel | Die ist uns gar nicht mehr relevant. |
| 01:00:45,359 | 01:00:46,120 | Marcel | Abweichung vom Konzept. |
| 01:00:48,400 | 01:00:51,040 | Marcel | Das Originalkonzept hat vorgesehen, dass die Mitgliedsnummer mit übermittelt wird, |
| 01:00:51,100 | 01:00:54,740 | Marcel | mit gespeichert wird und geprüft wird und dann jeder Verein sein eigenes Segment hat |
| 01:00:54,900 | 01:00:57,320 | Marcel | und jeder Verein eine eigene Verschlüsselungs-ID hat und so weiter. |
| 01:00:57,440 | 01:00:58,400 | Marcel | Das sind alles Abweichungen vom Ding. |
| 01:00:58,580 | 01:01:00,560 | Robert | Okay, aber das sind technische Details. |
| 01:01:00,780 | 01:01:02,600 | Marcel | Ja, Robert, genau. Aber da bin ich voll bei dir. |
| 01:01:02,740 | 01:01:04,320 | Marcel | Deswegen habe ich auch gesagt, Klaus, lass gut sein. |
| 01:01:04,440 | 01:01:12,600 | Marcel | Wir wissen, aber er hat das Gefühl, die Kontrolle zu verlieren, glaube ich, und hat eben kein Vertrauen. |
| 01:01:12,820 | 01:01:15,040 | Marcel | Und das habe ich auch gesagt. Das nervt mich, dass du da kein Vertrauen hast. |
| 01:01:15,620 | 01:01:18,620 | Marcel | Was haben wir denn in den letzten Jahren gemacht, was scheiße ist oder was nicht funktioniert? |
| 01:01:22,079 | 01:01:30,500 | Robert | Ich glaube, da müssen wir vielleicht die Use Cases, die wir festlegen, in Form von einem Dokument protokollieren. |
| 01:01:30,540 | 01:01:31,700 | Marcel | Wir protokollieren das auch. |
| 01:01:32,820 | 01:01:36,280 | Marcel | Ich würde bloß jetzt mal ein Blick auf die Zeit, also ich hätte, lieber treffen wir uns nochmal, |
| 01:01:36,740 | 01:01:38,840 | Marcel | eben auf zwei Stunden heute gedeckelt, also eine halbe Stunde noch. |
| 01:01:40,840 | 01:01:43,700 | Marcel | Und wir haben jetzt den Use Case im Endeffekt aufsperren, zusperren gehabt. |
| 01:01:45,380 | 01:01:50,160 | Marcel | Plus verlängern, plus den Sonder-Use Case, eine Buchung wird gelöscht |
| 01:01:51,160 | 01:01:56,320 | Marcel | und ein anderer übernimmt oder bucht zeitgleich nochmal oder ich lösche und darf nicht aufsperren können. |
| 01:01:56,380 | 01:02:05,880 | Marcel | Das waren so viele kleine Use Cases, die alle speziell aufs Zusperren, aber auch aufs Aufsperren zutreffen, die wir identifiziert haben mit Terminänderungen eigentlich. |
| 01:02:06,080 | 01:02:15,140 | Marcel | Wie gehen wir damit um? Also Use Case, User löscht seinen Termin, andere User legt ihn an zeitgleich oder ich verschiebe meine Buchung, damit ich mich trotzdem früher aufsperren kann. |
| 01:02:15,400 | 01:02:17,860 | Marcel | Wie gehen wir damit um? Da haben wir sehr viel drüber gesprochen. |
| 01:02:18,780 | 01:02:25,840 | Marcel | Ich bin gespannt, was der KI rausmacht. Ich versuche es gleich im Nachgang des Gesprächs noch aufzubereiten und der KI zu geben, dass du heute noch das bekommst, |
| 01:02:25,920 | 01:02:28,300 | Marcel | weil heute ist es noch frisch, da können wir beide drüber lesen und ergänzen. |
| 01:02:28,420 | 01:02:29,480 | Robert | Ich kann mal drüber lesen, ja. |
| 01:02:29,620 | 01:02:32,280 | Marcel | Ja, wenn wir es in zwei Wochen machen, dann weiß ich nicht mehr, was wir gequatscht haben. |
| 01:02:33,460 | 01:02:34,700 | Marcel | Genau, aber was haben wir noch für Use Cases? |
| 01:02:34,780 | 01:02:41,000 | Marcel | Wir haben Use Cases noch, also aufspannend zusperren sind eigentlich die zwei Use Cases, die die ZK hat, |
| 01:02:41,759 | 01:02:44,540 | Marcel | was jetzt wirklich Zugangskontrolle angeht. |
| 01:02:46,580 | 01:02:55,920 | Marcel | Use Cases, die es sonst noch gibt, ist: Eine Karte wird von einem Administrator gesperrt, entfernt. |
| 01:03:00,320 | 01:03:01,820 | Marcel | Also in deiner Software. |
| 01:03:03,060 | 01:03:08,340 | Marcel | Ich gehe rein, Führerschein wird verloren gemeldet, der Administrator geht rein und löscht die Karte. |
| 01:03:10,000 | 01:03:13,620 | Robert | Ist aus meiner Sicht genauso behandelt, wie wenn ich die Buchungsstelle hier. |
| 01:03:14,760 | 01:03:16,460 | Marcel | Genau, es ist eine Änderung der Buchung. |
| 01:03:17,540 | 01:03:24,280 | Marcel | Weil du musst dann hergehen, musst die Whitelist aller Termine, in denen diese UID drinsteht, |
| 01:03:24,820 | 01:03:27,680 | Marcel | musst du die rauslöchen, aber das kannst du ja, da es eine Datenbank ist, kannst du sagen, |
| 01:03:27,840 | 01:03:31,560 | Marcel | gib mir zu dieser UID alle Termine, wo sie drinsteht. |
| 01:03:32,700 | 01:03:34,040 | Marcel | Und dann musst du die da überall rauslöschen. |
| 01:03:35,980 | 01:03:41,720 | Marcel | Und aus dem zentralen Ding, beziehungsweise rauslöschen aus den Terminen, irgendwo anders mit abgelaufen, |
| 01:03:41,840 | 01:03:47,120 | Marcel | Das ist deine Datenhaltung, das ist nicht wichtig, aber es ist, glaube ich, schon gut zu wissen, zu wem hat die UID mal gehört. |
| 01:03:47,210 | 01:03:54,880 | Marcel | Also löschen aus der UID-Verwaltung würde ich sie nicht, aber aus den Terminen muss sie definitiv raus und sie darf halt nicht mehr in die globale Whitelist reinkommen. |
| 01:03:54,960 | 01:04:02,720 | Marcel | Also aus beiden Whitelisten aller Sachen muss sie raus und alle Termine, die dadurch tangiert werden, bei allen Autos, wo schon Buchungen drin sind, |
| 01:04:02,900 | 01:04:08,580 | Marcel | von den Mitgliedern oder dem Mitglied, wo die Karte registriert ist, müssen dann aktualisiert werden. |
| 01:04:09,500 | 01:04:13,920 | Robert | Genau. Jetzt hast du gerade gesagt, erlöschen aus der UID-Verwaltung würde ich sie nicht. |
| 01:04:16,700 | 01:04:17,660 | Marcel | Ja, Nachverfolgbarkeit. |
| 01:04:20,160 | 01:04:23,540 | Robert | Ja, also Nachverfolgbarkeit bezüglich Mitglieder ja. |
| 01:04:24,000 | 01:04:24,240 | Robert | Genau. |
| 01:04:24,380 | 01:04:28,060 | Robert | Ich lösche keine Mitglieder. Allein schon abgerechnet so ähnlich kann ich sie machen. |
| 01:04:28,160 | 01:04:37,100 | Marcel | Genau, aber die Frage ist, da du ja in den Logdaten, in den historischen Daten, das ist dann wieder Datenvorhaltung bei dir, insofern das betrifft die ZK nicht. |
| 01:04:38,759 | 01:04:43,720 | Marcel | In meiner Denke, wenn ich mit Buchungsdaten und irgendwas zu tun habe, |
| 01:04:44,340 | 01:04:46,060 | Marcel | und auch im zeitlichen Verlauf, |
| 01:04:46,820 | 01:04:48,360 | Marcel | ist es so, dass du unter Umständen sagst: |
| 01:04:48,380 | 01:04:53,880 | Marcel | "Oh, diese Karten-ID hat aber vor drei Monaten dieses Auto aufgesperrt." |
| 01:04:54,420 | 01:04:55,660 | Marcel | "Wer war denn das?" |
| 01:04:56,080 | 01:04:59,900 | Marcel | "Hast du den Datensatz der UID und der User-Zuordnung gelöscht, |
| 01:04:59,900 | 01:05:01,400 | Marcel | hast du keine Chance mehr rauszukriegen, |
| 01:05:01,400 | 01:05:05,919 | Marcel | wer im Protokolleintrag vor drei Monaten mit dieser Kartennummer diesen Eintrag erzeugt hat." |
| 01:05:07,120 | 01:05:15,840 | Robert | Ja, es ist halt so eine Grätsche zwischen Datenschutz und Datenverhaltung. |
| 01:05:16,060 | 01:05:20,460 | Robert | Also irgendwann, aber dann muss ich diese Nutzer auch einfach verzögert löschen. |
| 01:05:20,520 | 01:05:24,440 | Robert | Also die Mitglieder werde ich verzögert löschen. |
| 01:05:25,400 | 01:05:29,260 | Robert | Also mit einem halben Jahr oder einem Jahr Verzögerung lösche ich die Mitglieder, wenn |
| 01:05:29,340 | 01:05:29,960 | Robert | sie gekündigt haben. |
| 01:05:30,300 | 01:05:30,720 | Marcel | Ja klar. |
| 01:05:31,080 | 01:05:42,200 | Marcel | Und ein Nutzer hat das Problem, dass ein Nutzer ja, je nachdem wie du die Daten konstruiert hast, nur ein Feld, wenn es jetzt eine klassische Datenstruktur ist, einfache, |
| 01:05:42,660 | 01:05:51,960 | Marcel | hast du unter dem Mitglied eine Untertabelle, diese Untertabelle sind die Nutzer drin und der Nutzer hat mehrere Felder, Name, E-Mail, UID zum Beispiel, Geburtsdatum. |
| 01:05:52,860 | 01:05:58,620 | Marcel | Und wenn du die UID rauslöscht und überschreibst, musst du halt irgendwo dir merken, was vorher stand drin. |
| 01:05:59,920 | 01:06:04,840 | Marcel | Und da ist aus meiner Erfahrung in unserer Firma das Eleganteste ein Änderungsprotokoll. |
| 01:06:06,480 | 01:06:14,080 | Marcel | Das hilft dir nämlich an ganz vielen Stellen, dass du einfach sagst, bei Stammdaten, Mitglieder und Nutzern oder auch Autos, |
| 01:06:14,760 | 01:06:26,180 | Marcel | wird jede Änderung mit Alterwert und Wer hat es geändert, in eine separate Historisierungstabelle geschrieben, wo sie dann nach drei Monaten wieder gelöscht wird. |
| 01:06:27,700 | 01:06:31,240 | Robert | Event Sourcing ist der Fachbegriff dafür. |
| 01:06:31,540 | 01:06:37,480 | Robert | Aber es ist so interessant das Event Sourcing, würde ich mir auch schauen. |
| 01:06:37,720 | 01:06:40,540 | Robert | Da gibt es interessante Artikel dazu. |
| 01:06:41,400 | 01:06:45,120 | Robert | Also im Prinzip, dieses Konzept habe ich ja bei Buchungen. |
| 01:06:45,970 | 01:06:48,900 | Robert | Da habe ich ja die komplette Historie hinterlegt. |
| 01:06:50,290 | 01:06:55,940 | Robert | Ich habe es mir bis jetzt gespart bei Änderungen bei Benutzern und bei Fahrzeugen. |
| 01:06:57,540 | 01:07:00,860 | Robert | Weil ich den... Also mir ist klar, dass ihr das gleich in der Firma macht. |
| 01:07:01,560 | 01:07:03,000 | Speaker 3 | Ja, ja, da geht's. |
| 01:07:03,000 | 01:07:08,220 | Robert | Aber beim Harsharing, da habe ich eigentlich die Komplexität nicht. |
| 01:07:08,540 | 01:07:16,240 | Marcel | Genau. Überlegst dir, war nur eine Idee meinerseits, dass man das nicht aus dem Auge verliert. |
| 01:07:19,339 | 01:07:29,800 | Robert | Die Frage ist eher: Siehst du Use-Faces, wo ich im Nachhinein feststellen kann, wer hat das geändert und wann? |
| 01:07:30,360 | 01:07:36,500 | Marcel | Das hängt an diesem Fall. |
| 01:07:36,600 | 01:07:37,520 | Marcel | wirst du haben, |
| 01:07:38,580 | 01:07:40,300 | Marcel | wie schlimm es ist, wenn du ihn nicht abbilden kannst, |
| 01:07:40,310 | 01:07:42,300 | Marcel | aber die Fragestellung wirst du überall da haben, |
| 01:07:42,370 | 01:07:43,700 | Marcel | wo du in größeren Vereinen |
| 01:07:44,420 | 01:07:46,000 | Marcel | mehr als einen Administrator hast. |
| 01:07:48,520 | 01:07:49,820 | Marcel | Und dann werden Daten geändert. |
| 01:07:50,580 | 01:07:51,360 | Marcel | Auf Zuruf. |
| 01:07:52,060 | 01:07:53,780 | Marcel | Weil Vereine arbeiten gerne auf Zuruf. |
| 01:07:54,260 | 01:07:55,900 | Marcel | Und protokollieren nicht sonderlich viel. |
| 01:07:56,940 | 01:07:57,620 | Marcel | Und wenn dann |
| 01:07:58,490 | 01:08:00,500 | Marcel | der K-Chef X bei dem Administrator |
| 01:08:00,600 | 01:08:01,980 | Marcel | anruft, du sagst, kannst du mal kurz. |
| 01:08:03,160 | 01:08:03,880 | Marcel | Dann macht der das. |
| 01:08:05,360 | 01:08:05,999 | Marcel | Und dann schaut |
| 01:08:07,560 | 01:08:08,900 | Marcel | der Klaus Breindl, |
| 01:08:09,280 | 01:08:10,080 | Marcel | der, weiß ich nicht, |
| 01:08:11,100 | 01:08:12,720 | Marcel | Marcel Mayer als Administrator |
| 01:08:12,780 | 01:08:13,880 | Marcel | auch mal rein und sagt, hä? |
| 01:08:14,780 | 01:08:16,799 | Marcel | Seit wann steht denn bei diesem Auto eine andere |
| 01:08:18,240 | 01:08:18,960 | Marcel | Abrechnungsnutzer-Zuordnung |
| 01:08:19,120 | 01:08:20,880 | Marcel | drin? Warum ist denn das jetzt in den Tarif |
| 01:08:21,240 | 01:08:22,620 | Marcel | 50 Cent verschoben, statt |
| 01:08:22,759 | 01:08:23,660 | Marcel | 1,50 Euro die Stunde? |
| 01:08:26,600 | 01:08:27,020 | Marcel | Und dann geht's |
| 01:08:27,100 | 01:08:28,660 | Marcel | darum rauszukriegen, wir hatten das gemacht. |
| 01:08:30,359 | 01:08:30,839 | Robert | Ja, aber |
| 01:08:31,020 | 01:08:31,839 | Robert | was bringt dir die Info? |
| 01:08:33,380 | 01:08:34,799 | Marcel | Die Info bringt mir, dass ich |
| 01:08:34,880 | 01:08:36,620 | Marcel | denjenigen jetzt bewusst nehmen kann und sagen kann, |
| 01:08:36,710 | 01:08:38,880 | Marcel | Leute, da ist was falsch gegangen. Und ich kann vor allen Dingen |
| 01:08:39,160 | 01:08:40,859 | Marcel | einschätzen, seit wann |
| 01:08:41,020 | 01:08:42,779 | Marcel | läuft denn das falsch? Wie viele Monate |
| 01:08:42,960 | 01:08:44,400 | Marcel | Abrechnungen haben wir denn jetzt falsch rausgeschickt? |
| 01:08:44,470 | 01:08:46,400 | Robert | Ja, okay, genau, das ist eher der Punkt. |
| 01:08:49,620 | 01:08:50,960 | Robert | Wobei du das alles herschneidest. |
| 01:08:51,339 | 01:08:53,120 | Marcel | Und deswegen gibt es immer die zwei Optionen, |
| 01:08:53,299 | 01:08:54,779 | Marcel | entweder Protokoll schreiben für die |
| 01:08:56,020 | 01:08:56,740 | Marcel | Stammdatenänderung oder |
| 01:08:57,020 | 01:08:59,040 | Marcel | alternativ die relevanten Stammdaten |
| 01:08:59,200 | 01:09:00,940 | Marcel | an Bewegungsdatensätzen mitschreiben. |
| 01:09:01,049 | 01:09:02,940 | Marcel | Das heißt, dann schreibst du halt an |
| 01:09:02,960 | 01:09:08,380 | Marcel | jeden Datensatz einer abgeschlossenen Buchung mit hin, diese Ressource mit diesem Stundenschlüssel, |
| 01:09:08,870 | 01:09:15,440 | Marcel | mit dem Benutzer, mit der UID der Karte wurde aufgesperrt, mit dem Benutzer und der UID der Karte wurde zugesperrt. |
| 01:09:16,259 | 01:09:21,940 | Marcel | Wenn du diese Informationen quasi redundant dort vorhältst, auch dem Benutzer, nicht nur die UID, |
| 01:09:22,900 | 01:09:24,920 | Marcel | ist es auch kein Thema, weil dann kannst du das daraus wieder. |
| 01:09:24,920 | 01:09:32,759 | Marcel | Dann hast du die Bewegungsdaten und siehst, die letzten zwölf Buchungen sind alle mit 50 Cent die Stunde statt 1,50 Euro die Stunde gebucht worden. |
| 01:09:34,120 | 01:09:38,380 | Marcel | und hier stehen die Benutzernamen, Klartext in den Buchungsinformationen drin. |
| 01:09:39,220 | 01:09:42,220 | Marcel | Und damit ist es mir egal, was der für eine UID zu dem Wert hat. |
| 01:09:43,100 | 01:09:45,220 | Robert | Also wenn, dann würde ich das als Protokoll machen. |
| 01:09:49,120 | 01:09:51,620 | Robert | Wobei, ja, das kann ich auf jeden Fall nachrüsten. |
| 01:09:52,319 | 01:09:57,080 | Marcel | Überlegst dir, wir machen tatsächlich beides in ganz vielen Fällen. |
| 01:09:57,660 | 01:09:58,940 | Marcel | Aber das hat Performance-Bund auch. |
| 01:09:59,960 | 01:10:03,240 | Robert | Ok, also das Protokoll ist mit Sicherheit das Elegante. |
| 01:10:05,100 | 01:10:12,020 | Marcel | Ja, ist weniger Daten, aber ist natürlich komplizierter zum Auswerten. |
| 01:10:15,480 | 01:10:21,100 | Robert | Ja, wobei die Auswertung, also den Use Case Auswertung, den hatte ich bis jetzt ja noch gar nicht. |
| 01:10:21,170 | 01:10:28,320 | Robert | Also der muss jetzt erst einmal kommen, und wenn er kommt, dann überlege ich mir das alles zu bauen, aber dann würde es erst voll machen. |
| 01:10:28,900 | 01:10:33,480 | Marcel | Passt. Und mit modernen Daten haben wir kein Thema, das zu joinen und zusammenzuführen und zu machen. |
| 01:10:33,940 | 01:10:34,140 | Robert | Genau. |
| 01:10:35,480 | 01:10:38,200 | Robert | Okay, gut, aber dann kommen wir zurück zu unserem Use Case. |
| 01:10:40,280 | 01:10:44,880 | Marcel | Also wir haben gesagt, jetzt der nächste Use Case, den wir nach Auf- und Zusperren in diversen Varianten hatten |
| 01:10:44,880 | 01:10:51,320 | Marcel | und Terminänderungen, Optionen bei dem dazu war, jetzt ich lösche eine Karte oder sperre sie. |
| 01:10:51,320 | 01:10:55,440 | Marcel | Heißt, ich nehme sie bei einem Nutzer raus. |
| 01:10:56,120 | 01:11:15,340 | Marcel | Und damit muss diese Karte, typischerweise tausche ich sie vielleicht sogar gleich gegen eine neue, muss diese alte Nummer aber bei allen Mitgliedern, die mit diesem Nutzer verknüpft sind und allen Terminen, die an diesen Mitgliedern hängen, müssen die Whitelisten der Termine aktualisiert werden und zusätzlich natürlich die globale Whitelist. |
| 01:11:16,480 | 01:11:34,360 | Marcel | Das ist ein Fall und das heißt dann wiederum, dass das Ändern der Whitelist eines Termines, die vorher genannte Abfrage, hat sich was geändert, ja, nein, bei den einzelnen Ressourcen, die gebucht wurden, als Ja zurückgeben muss und dann müssen die sich aktualisieren, kriegen die neue Whitelist und haben damit die neue Whitelist drin. |
| 01:11:34,460 | 01:11:47,760 | Marcel | Und da ist es wiederum so, dass ich sage, ich würde ohnehin einmal täglich mit jedem Auto, egal ob es eine Buchung hat oder nicht, die Buchungen abrufen, damit man eben auch genau den Fall abfängt, dass man die Whitelist dort, weil die selbst das brauchst du ja gar nicht, stimmt. |
| 01:11:47,980 | 01:11:53,280 | Marcel | Wir haben ja gesagt, wir fragen fünf Minuten bevor der Termin anfängt, fragen wir eh nochmal ab, gibt es eine Änderung. |
| 01:11:53,660 | 01:12:00,340 | Marcel | Damit hätten wir das, ich lösche eine ID oder tausche sie gegen eine neue, hätten wir damit abgefrühstückt. |
| 01:12:01,280 | 01:12:03,940 | Robert | Also eine Minute oder eigentlich so kurz, wie es geht. |
| 01:12:04,240 | 01:12:04,280 | Robert | Ja. |
| 01:12:04,620 | 01:12:05,880 | Robert | Bevor die Bogen losgeht, oder? |
| 01:12:05,940 | 01:12:06,020 | Robert | Genau. |
| 01:12:07,140 | 01:12:07,580 | Robert | Genau. |
| 01:12:07,810 | 01:12:19,900 | Robert | Und die, ähm, genau, damit verhindern wir das immer, das Öffnen lange dauert, das war |
| 01:12:19,980 | 01:12:27,240 | Robert | das eine, und das zweite, damit wir eine Stornierung, deswegen haben wir ja gesagt, wenn wir es wiederholen |
| 01:12:27,240 | 01:12:32,940 | Robert | dann, also alle Viertelstunde, damit wir Stornierungen rechtzeitig mitkriegen und man |
| 01:12:32,960 | 01:12:38,900 | Robert | ein Auto bucht und dann storniert und dann trotzdem öffnet. |
| 01:12:40,380 | 01:12:45,640 | Robert | Wobei ich gestehen muss, da gibt es eine Kölkskunde, anders mache ich. |
| 01:12:45,640 | 01:12:51,880 | Robert | Ich kann es buchen, geht es im Auto, sperre es auf, aber dann ist es protokolliert, dass ich aufgesperrt habe. |
| 01:12:52,160 | 01:12:52,680 | Marcel | Ja eben. |
| 01:12:54,220 | 01:12:55,940 | Robert | Aber das ist sowieso protokolliert. |
| 01:12:56,600 | 01:13:00,840 | Marcel | Es geht weniger um den Fall, ich möchte das Auto verbetrügen, wie das Auto mobsen. |
| 01:13:01,340 | 01:13:03,300 | Marcel | Weil es ist protokolliert, wer es aufgesperrt hat. Punkt. |
| 01:13:04,660 | 01:13:11,540 | Marcel | Also wenn ich mutwillig bin, sperre ich es mit der Karte auf, reiße die ZK raus, schmeiße sie in den Müllarmal, löse sie in Essigsäure auf, keine Ahnung was. |
| 01:13:11,960 | 01:13:13,560 | Marcel | Essigsäure funktioniert nicht. Whatever. |
| 01:13:14,200 | 01:13:16,820 | Marcel | Das ist Quatsch, weil von diesen kriminellen Machen schaffen gehen wir nicht aus. |
| 01:13:16,890 | 01:13:21,540 | Marcel | Aber der Use Case, der durchaus vorkommt, heißt: Ich studiere eine Buchung. |
| 01:13:25,120 | 01:13:26,780 | Robert | Und wie kriegt die ZK mit? |
| 01:13:26,960 | 01:13:32,900 | Robert | Aber der Ehemann storniert, die Frau charaketiert und leitet trotzdem aus. |
| 01:13:33,120 | 01:13:34,060 | Robert | Das wäre glaube ich ein Klassiker. |
| 01:13:34,760 | 01:13:34,960 | Robert | Ja. |
| 01:13:35,260 | 01:13:38,600 | Robert | Die zwei von einem Haushalt, die zwei reden miteinander. |
| 01:13:39,520 | 01:13:40,340 | Marcel | Reden aneinander vorbei. |
| 01:13:41,390 | 01:13:41,500 | Marcel | Genau. |
| 01:13:41,830 | 01:13:43,580 | Marcel | Aber deswegen ist es gut, wenn wir aktualisieren. |
| 01:13:43,610 | 01:13:49,580 | Marcel | Aber der wahrscheinlichere Fall ist, dass ich einfach, weil das passiert, eine Buchung am gleichen Tag storniere, |
| 01:13:49,610 | 01:13:54,140 | Marcel | weil ich mir denke, ach Mist, oder acht Stunden reserviere und nach vier Stunden wieder da bin, |
| 01:13:54,150 | 01:13:56,580 | Marcel | war es geil, lief mein Termin und ich bin super schnell durchgekommen. |
| 01:13:57,000 | 01:14:04,960 | Marcel | Dann stelle ich das Auto hin, gebe den Rest der Zeit frei und jemand anderes nimmt sich das, bucht es und will losfahren. |
| 01:14:05,110 | 01:14:05,940 | Marcel | Und das muss funktionieren. |
| 01:14:07,080 | 01:14:09,480 | Robert | Nein, Moment. Der Fall ist abgedeckt. |
| 01:14:09,570 | 01:14:18,580 | Robert | Weil wenn ich nach 4 Stunden zurückkomme, das Auto abgibt und dann kommt der andere, der es gebucht hat, |
| 01:14:18,910 | 01:14:21,620 | Robert | dann sagt das Auto erstmal "stopp, das geht ja nicht". |
| 01:14:21,680 | 01:14:23,140 | Robert | Da hat der andere reserviert. |
| 01:14:23,760 | 01:14:27,900 | Robert | Aber weil wir ja die Logik ändern, würde er trotzdem beim Hintergrundsystem aufräumen. |
| 01:14:28,420 | 01:14:32,000 | Robert | Und dann sehen: "Ah, stopp, das ist ja erdeckt inzwischen." |
| 01:14:32,560 | 01:14:35,080 | Marcel | Es würde noch anders funktionieren nach unserer Definition. |
| 01:14:35,240 | 01:14:36,760 | Marcel | Weil in dem Moment, wo ich absperre, |
| 01:14:37,980 | 01:14:39,780 | Marcel | gehst du mal davon aus, das Auto steht jetzt. |
| 01:14:39,900 | 01:14:41,940 | Marcel | Wenn das Auto steht und es ist eine Reservierung aktiv, |
| 01:14:42,100 | 01:14:45,640 | Marcel | fragst du alle x Minuten den Server an, gibt es eine Terminänderung. |
| 01:14:49,700 | 01:15:02,620 | Marcel | Weil wir hatten definiert Zustand Auto abgesperrt, per ZK Kommando abgesperrt und aktiver Termin heißt, ich prüfe regelmäßig, ob der Termin nicht gelöscht wurde oder geändert. |
| 01:15:04,620 | 01:15:10,440 | Marcel | Also während eines laufenden Reservierungsfensters im Zustand abgesperrt prüfe ich immer, gibt es den Termin überhaupt noch? |
| 01:15:12,020 | 01:15:15,280 | Robert | Moment, äh, nein, kann man vielleicht... |
| 01:15:16,100 | 01:15:19,800 | Robert | Also ich bin davon ausgegangen, dass man, wenn ich einen Termin habe, |
| 01:15:20,270 | 01:15:22,820 | Robert | dann tue ich eine Minute bevor der Termin stattfindet, |
| 01:15:23,570 | 01:15:25,920 | Robert | das nochmal kontrollieren, ob es nach wie vor stimmt. |
| 01:15:26,230 | 01:15:26,340 | Marcel | Genau. |
| 01:15:27,500 | 01:15:31,780 | Robert | Wenn ich keinen Termin habe, dann mache ich nichts. |
| 01:15:32,030 | 01:15:33,880 | Robert | Also, neuerndlich, klar, mache ich Aktualisierung, |
| 01:15:34,500 | 01:15:37,840 | Robert | aber ansonsten ist meist dann jetzt, dass wir dann nichts machen. |
| 01:15:38,080 | 01:15:38,260 | Marcel | Genau. |
| 01:15:39,600 | 01:15:44,340 | Marcel | Jetzt habe ich aber so, lass uns diesen Fall zeitlich chronologisch durchgehen. |
| 01:15:44,960 | 01:15:52,620 | Marcel | Ich habe mir eine Buchung gemacht, egal wann, für morgens 8 bis abends 18 Uhr, 10 Stunden reserviertes Auto für meine Fahrt zum Kunden. |
| 01:15:55,180 | 01:16:03,620 | Marcel | Dann steige ich morgens in das Auto rein, komme um 10 nach 8 hin, um eine Minute vor 8 hat er nochmal eine Abfrage gemacht, kriegt mit, nein, es hat sich nichts geändert. |
| 01:16:05,620 | 01:16:13,360 | Marcel | Und dann, ab da hast du ja gesagt, auch während der Termin schon läuft, also meine Reservierung aktiv ist, kann es ja sein, dass ich währenddessen den Termin lösche. |
| 01:16:13,730 | 01:16:20,160 | Marcel | Das heißt, du frechst, bis ich aufsperre, alle Viertelstunde, 5 Minuten, Zeitfenster muss man noch definieren, den Surfer an, gibt es eine Änderung? |
| 01:16:20,800 | 01:16:23,440 | Marcel | Nö. Gibt es eine Änderung? Nö. Gibt es eine Änderung? Nö. |
| 01:16:23,700 | 01:16:28,640 | Marcel | Irgendwann um Viertel nach 8 sperre ich das Auto auf und steige ein, fahre los. |
| 01:16:28,900 | 01:16:33,720 | Marcel | Ab da sagt die ZK, Auto ist aufgesperrt, ich brauche nichts mehr tun, Füße hochlegen. |
| 01:16:34,680 | 01:16:44,520 | Marcel | Irgendwann komme ich wieder heim und zwar nicht wie geplant um 18 Uhr, so wie ich reserviert habe, sondern schon um 14 Uhr, weil der Kunde hat mich heimgeschickt, ich war zu dumm, was auch immer passiert ist. |
| 01:16:44,930 | 01:16:51,620 | Marcel | Ich bin um 14 Uhr da, stelle das Auto ab, ändere meine Buchung und sage, nein, Buchungsende ist jetzt 14 Uhr. |
| 01:16:53,040 | 01:16:58,500 | Marcel | Dann bin ich aber wieder in ZK, sieht ja nur, ich habe von C8 bis 18 Uhr reserviert. |
| 01:16:58,580 | 01:16:59,760 | Marcel | Jetzt sperre ich das Auto ab. |
| 01:17:00,640 | 01:17:01,920 | Marcel | Damit greift wieder die Regel. |
| 01:17:01,970 | 01:17:03,420 | Marcel | Das Auto ist abgesperrt. |
| 01:17:04,080 | 01:17:05,980 | Marcel | Und ich bin in einer aktiven Buchung. |
| 01:17:06,320 | 01:17:12,040 | Marcel | Also frage ich alle x Minuten, 10, 15 Minuten, den Sharepad an. |
| 01:17:12,260 | 01:17:13,180 | Marcel | Gibt es eine Änderung? |
| 01:17:13,900 | 01:17:16,900 | Marcel | Und dann würde der Sharepad sagen, ja, es gibt eine Änderung. |
| 01:17:16,990 | 01:17:17,400 | Marcel | Hier sind. |
| 01:17:17,510 | 01:17:19,020 | Marcel | Und dann sage ich, oh, es gibt eine Änderung. |
| 01:17:19,450 | 01:17:20,280 | Marcel | Gib mir mal die Termine. |
| 01:17:20,440 | 01:17:22,280 | Marcel | Und dann lädt die ZK die Termine runter. |
| 01:17:22,840 | 01:17:24,540 | Marcel | Und auf einmal ist die Buchung um 14 Uhr zu Ende. |
| 01:17:25,120 | 01:17:27,700 | Marcel | Und um 14.30 Uhr beginnt eine Neubuchung. |
| 01:17:29,160 | 01:17:30,360 | Marcel | Oder ist es noch keine Buchung da? |
| 01:17:32,200 | 01:17:35,080 | Robert | Aber dann muss die ZK sich den Zustand merken. |
| 01:17:36,760 | 01:17:37,520 | Robert | Aufgesperrt oder nicht? |
| 01:17:37,560 | 01:17:38,260 | Robert | Das weiß sie. |
| 01:17:41,040 | 01:17:42,180 | Marcel | Den Zustand kennt die ZK. |
| 01:17:42,840 | 01:17:43,780 | Marcel | Weil das ist ja ein Flipflop. |
| 01:17:44,000 | 01:17:46,540 | Marcel | Wenn ich die Karte drauflege, wird einmal zugesperrt. |
| 01:17:46,540 | 01:17:48,220 | Marcel | Wenn ich die gleiche Karte wieder drauflege, ist es aufgesperrt. |
| 01:17:48,220 | 01:17:50,080 | Marcel | Das heißt, die ZK weiß, in welchem Zustand sie ist. |
| 01:17:51,880 | 01:17:53,060 | Robert | Wobei der ja nicht sicher ist. |
| 01:17:53,360 | 01:17:55,620 | Robert | Also ich konnte zusperren und bin trotzdem aufgesperrt. |
| 01:17:57,820 | 01:18:01,680 | Marcel | Wenn ich das mit dem Schlüssel mache, ja, aber dann ist es kein buchungstechnisch relevantes Zusperren. |
| 01:18:05,720 | 01:18:07,680 | Robert | Das Auto wird dann trotzdem ständig pollen. |
| 01:18:08,720 | 01:18:10,020 | Marcel | Nein, nein, nein, nein. |
| 01:18:10,740 | 01:18:17,420 | Marcel | Wenn es so lange das Auto aufgesperrt ist per ZK-Definition, also nur das Karten auf- und zusperren zählt ja. |
| 01:18:17,420 | 01:18:23,500 | Marcel | Wenn ich es mit der Karte aufsperre, ab dem Moment pingt er nicht mehr an, bis zu dem Moment, wo ich es mit der Karte wieder zusperre. |
| 01:18:24,560 | 01:18:27,200 | Marcel | und eine Buchung noch aktiv ist, also eine Reservierung. |
| 01:18:28,030 | 01:18:30,960 | Marcel | Ab da bringt er wieder den Säufer an und sagt: "Gibt es Buchungsänderungen?" |
| 01:18:31,780 | 01:18:33,960 | Marcel | bis Terminende. Ab Terminende macht er das nicht mehr. |
| 01:18:44,920 | 01:18:45,360 | Robert | Okay. |
| 01:18:46,280 | 01:18:49,160 | Marcel | Also das wäre aus meiner Sicht eine sehr klare, einfache Regel. |
| 01:18:51,620 | 01:18:57,780 | Robert | Ja, die Regel ist einfach. Was wir erklären müssen ist das mit dem Stromverbrauch. |
| 01:18:58,510 | 01:19:04,700 | Robert | Aber klar, das haben wir ja schon gesagt, das ist ein Dufländchen. |
| 01:19:05,180 | 01:19:05,320 | Robert | Ja. |
| 01:19:08,460 | 01:19:16,660 | Robert | Es hätte vielleicht auch den Vorteil, dass wir anstatt nächtlich, dass wir uns vielleicht sogar das nächtliche Aktualisieren komplett sparen können. |
| 01:19:17,820 | 01:19:23,380 | Robert | und dass ich als Server tatsächlich pushen kann. |
| 01:19:24,720 | 01:19:25,520 | Robert | Ja, indirekt. |
| 01:19:25,820 | 01:19:31,240 | Robert | Allein dadurch, dass du ständig pollst, kann ich sagen: "So, ich beschließe jetzt." |
| 01:19:31,580 | 01:19:34,920 | Robert | Ich kann ja trotzdem sagen, ich tue nur in der Nacht updaten. Das ist meine Entscheidung dann wieder. |
| 01:19:35,560 | 01:19:40,260 | Robert | Und das gefällt mir so gut an der Lösung, dass ich die komplette Entscheidungshoheit kriege. |
| 01:19:41,020 | 01:19:42,120 | Marcel | Mhm, das stimmt. Ja. |
| 01:19:42,880 | 01:19:51,480 | Marcel | Dann würde es aber tatsächlich so machen, dass man sagt, es gibt, also die ZK pinkt dich immer an auf Änderungen. |
| 01:19:51,640 | 01:19:57,100 | Marcel | Und das macht sie immer viertelstündlich oder stündlich oder sowas in einem eher längeren Intervall. |
| 01:19:58,960 | 01:20:05,120 | Marcel | Und während aktiven Terminen oder ab 5 Minuten vor dem aktiven Termin, den die ZK kennt, |
| 01:20:05,700 | 01:20:10,560 | Marcel | macht sie es dann nicht mehr im Halbstunden- oder Stundenrhythmus, sondern im 5-Minuten-Rhythmus, dass sie dich anpinkt. |
| 01:20:12,140 | 01:20:13,280 | Marcel | Bis der Termin zu Ende ist. |
| 01:20:14,200 | 01:20:17,200 | Marcel | Weil, es ist ja davon auszugehen, dass auch der Stromverbrauch dann ehrlich gesagt scheißegal, |
| 01:20:17,500 | 01:20:21,840 | Marcel | ist ja auch davon auszugehen, dass wenn der Termin wahrgenommen wird, fährt das Auto und damit lädt die Batterie eh wieder. |
| 01:20:22,620 | 01:20:26,860 | Marcel | Damit ist es wurscht, dass sie während des Termins und vielleicht die zwei Stunden danach noch aktiv pingt, |
| 01:20:26,860 | 01:20:28,320 | Marcel | weil so viel Strom braucht es nicht. |
| 01:20:29,160 | 01:20:36,260 | Robert | Was ist das Problem von den 5 Minuten Zykluspingen? |
| 01:20:37,060 | 01:20:49,640 | Marcel | Das hat den Vorteil, dass ich eine Terminänderung für "Ich lösche den Termin" und jemand anders bucht ihn, gerade zu stark gefragten Zeiten, dass ich das sehr schnell mitbekomme und dann der User nicht davor stehen muss und warten muss, dass er es jetzt aufsperren kann. |
| 01:20:50,540 | 01:20:51,480 | Robert | Wie oft haben wir das? |
| 01:20:53,940 | 01:20:57,460 | Marcel | Wie oft nachgebucht wird, weiß ich nicht, aber Terminverkürzungen... |
| 01:20:57,460 | 01:20:58,140 | Marcel | Ich kann es rausfinden. |
| 01:20:59,160 | 01:20:59,980 | Marcel | Wenn du rausfinden kannst. |
| 01:21:00,020 | 01:21:00,800 | Robert | Lass mir das rausführen. |
| 01:21:00,820 | 01:21:00,940 | Robert | Ja. |
| 01:21:02,020 | 01:21:06,460 | Robert | Ja, die Frage ist, weil das ist ja nur relevant, wenn ich wirklich kurzfristig buche. |
| 01:21:07,140 | 01:21:12,300 | Robert | Also ich storniere kurzfristig und der andere bucht kurzfristig und zwar sofort jetzt. |
| 01:21:12,620 | 01:21:13,620 | Robert | Ja genau, also ich hätte eben... |
| 01:21:13,620 | 01:21:14,380 | Robert | Die Fälle sind relevant. |
| 01:21:14,880 | 01:21:18,540 | Marcel | Ja, also wenn man sagt, man macht sonst einen stundenweise Ping, dann hast du recht. |
| 01:21:18,600 | 01:21:20,920 | Marcel | Das muss innerhalb der einen Stunde passieren. |
| 01:21:22,120 | 01:21:24,160 | Robert | Ich lasse mir schauen, ich tue ein paar Zeiten raus. |
| 01:21:24,920 | 01:21:28,180 | Robert | Also ich prüfe es mir mit einer Viertelstunde, Halbestunde, Stunde, Stunde und so weiter. |
| 01:21:28,620 | 01:21:38,740 | Robert | Also die Vorzeiten sozusagen und dann kann man schauen, wie oft, es geht ja nur darum, |
| 01:21:40,920 | 01:21:45,380 | Robert | die Zeiten, wo ein Server eraufragen muss, wenn der Benutzer die Karte hat. |
| 01:21:45,600 | 01:21:53,780 | Marcel | Genau, dass er bei dem Drauflegen anfragen muss, dieses Intervall sollten wir möglichst wenig halten. |
| 01:21:54,620 | 01:21:56,400 | Marcel | Weil 99% der Fälle... |
| 01:21:56,570 | 01:21:57,520 | Robert | Wie lange dauert das? |
| 01:21:57,720 | 01:21:59,900 | Robert | Wie oft passiert das? |
| 01:22:00,150 | 01:22:00,780 | Marcel | Genau, perfekt. |
| 01:22:02,720 | 01:22:04,560 | Marcel | Wie gesagt, das war ja ein To-Do, was wir schon hatten. |
| 01:22:04,690 | 01:22:06,260 | Marcel | Aber jetzt gerade nochmal für mich zum Wiederholen. |
| 01:22:06,880 | 01:22:09,320 | Marcel | Finn-Fragen, wie lange braucht es von |
| 01:22:09,430 | 01:22:10,520 | Marcel | ich schlafe auf |
| 01:22:10,710 | 01:22:11,820 | Marcel | ich habe eine Internetverbindung, |
| 01:22:12,420 | 01:22:13,380 | Marcel | wie lange braucht es von |
| 01:22:13,620 | 01:22:15,320 | Marcel | Modem ist zwar Strom versorgt, aber hat kein Internet, |
| 01:22:15,390 | 01:22:17,240 | Marcel | bis die Internetverbindung steht und ich den ersten |
| 01:22:17,440 | 01:22:19,560 | Marcel | Request machen kann, und wie viel Strom |
| 01:22:19,900 | 01:22:21,820 | Marcel | braucht es, wenn die Internetverbindung steht |
| 01:22:22,180 | 01:22:23,200 | Marcel | und ich sie gar nicht schließe, |
| 01:22:23,380 | 01:22:24,600 | Marcel | sondern die Connection Live halte. |
| 01:22:25,180 | 01:22:27,440 | Marcel | Im Vergleich zu, ich mache die Internetverbindung |
| 01:22:27,580 | 01:22:29,280 | Marcel | und habe nur das Modem Strom versorgt. |
| 01:22:29,360 | 01:22:31,460 | Marcel | Im Vergleich zu, ich schicke das Modem in den Schlafmodus. |
| 01:22:32,220 | 01:22:33,180 | Robert | Das sind die drei |
| 01:22:33,640 | 01:22:35,060 | Marcel | Stromverbräuche, die uns interessieren. |
| 01:22:35,120 | 01:22:37,640 | Marcel | Und dann die Zeitübergänge zwischen den verschiedenen Zuständen |
| 01:22:38,420 | 01:22:39,380 | Marcel | wären auch noch interessant. |
| 01:22:40,820 | 01:22:40,980 | Marcel | Also, |
| 01:22:41,400 | 01:22:43,400 | Marcel | wie lange brauche ich vom Schlafmodus, bis der erste |
| 01:22:43,560 | 01:22:44,480 | Marcel | Request ins Internet geht? |
| 01:22:45,140 | 01:22:45,640 | Marcel | Also ein Beispiel. |
| 01:22:47,760 | 01:22:48,440 | Marcel | Passt. Okay. |
| 01:22:49,180 | 01:22:51,540 | Marcel | Jetzt hatten wir den Use Case Karte sperren. |
| 01:22:51,720 | 01:22:53,340 | Marcel | Sind nochmal zurückgehüpft auf die |
| 01:22:55,640 | 01:22:57,580 | Robert | Entschuldigung, wenn ich kurz weitergehe. |
| 01:22:57,920 | 01:23:02,260 | Robert | Nicht bis der erste Request ins Internet geht, sondern bis so ein Minimal-Request beantwortet wird. |
| 01:23:02,820 | 01:23:06,040 | Robert | Nur damit wir dann auch den kompletten Turnaround mit drin haben. |
| 01:23:06,440 | 01:23:11,060 | Marcel | Gut, den Turnaround kann er ja nicht berechnen. Er kann nur sagen, wie lange brauche ich, bis ich meinen ersten Ping zu Aldi hinschicke. |
| 01:23:15,420 | 01:23:19,720 | Marcel | Weil wie lange du für die Antwort des Requests brauchst, hängt ja am Server an ganz vielen anderen Faktoren. |
| 01:23:19,720 | 01:23:25,120 | Marcel | Aber wenn er jetzt zum Beispiel, ich weiß nicht ob die Library das kann, einen Ping schickt an www.aldi.de, |
| 01:23:26,520 | 01:23:31,180 | Marcel | quasi bin ich bei dir irgendwas Minimalses, was Verarbeitungszeit auf der Gegenseite gegen Null hat, |
| 01:23:32,200 | 01:23:35,320 | Marcel | wie lange braucht er, bis er die Antwort zum Beispiel auf einen Ping bekommt an aldi.de. |
| 01:23:36,260 | 01:23:37,080 | Robert | Ja, okay, gut. |
| 01:23:37,470 | 01:23:38,060 | Marcel | Das wäre die Zeit. |
| 01:23:39,040 | 01:23:41,120 | Robert | Im Recht ist schon ein bisschen der Standard. |
| 01:23:41,570 | 01:23:42,740 | Marcel | Ja, okay. |
| 01:23:44,040 | 01:23:47,060 | Marcel | Wir hatten Karte löschen, aufsperren, zusperren, Terminänderungen. |
| 01:23:48,320 | 01:23:56,260 | Robert | Ehm, ein Use Case, den wir noch haben oder nicht hatten, ist, äh, Auto registrieren. |
| 01:23:58,700 | 01:23:59,120 | Speaker 3 | Mhm. |
| 01:23:59,620 | 01:24:07,120 | Robert | Also, wenn meine Vorstellung wäre, ehrlich gesagt, dass wir die Boxen, wenn wir sie zusammenbauen, |
| 01:24:07,530 | 01:24:12,780 | Robert | dann wäre da die Fernweh programmiert und dass wir da, äh, jetzt kommen wir wieder zu diesem Thema ID, |
| 01:24:13,600 | 01:24:16,180 | Robert | dass wir da eine eindeutige ID reinladen. |
| 01:24:18,020 | 01:24:19,200 | Marcel | Die Frage ist: |
| 01:24:20,320 | 01:24:21,880 | Marcel | Also ich würde jetzt, da wir noch |
| 01:24:23,760 | 01:24:26,820 | Marcel | theoretisch sechs Minuten haben, bis ich sage, ich würde gern Feierabend machen, |
| 01:24:27,720 | 01:24:32,440 | Marcel | vielleicht jetzt mich darauf beschränken, dass wir nochmal uns die Use Cases, die wir haben, kurz auflisten |
| 01:24:33,180 | 01:24:37,060 | Marcel | und als To-Do fürs nächste Mal mitnehmen, dass wir die dann beim nächsten Mal strukturiert durchgehen. |
| 01:24:37,980 | 01:24:40,920 | Marcel | Also du hast gesagt: Autoregistrieren ist ein Use Case. |
| 01:24:41,540 | 01:24:49,860 | Marcel | Sprich, das Auto kommt neu zu einem Autoteilerverein, ich habe eine ZK irgendwo zusammengebaut, vorkonfiguriert, fahre ich schon in der Schublade liegen und bau dir das Auto ein. |
| 01:24:50,340 | 01:24:56,500 | Marcel | Und wie verheirate ich diese ZK mit der Software drauf, mit dem Auto, mit der Ressource und dem Verein? |
| 01:24:57,280 | 01:24:58,660 | Marcel | Das ist ein Use Case, den wir durchdenken müssen. |
| 01:24:59,700 | 01:25:04,060 | Robert | Verheiraten, genau. Und der andere Use Case ist Konfigurieren oder Änderung der Konfiguration. |
| 01:25:05,420 | 01:25:10,400 | Robert | Das eine ist, dass ich sage, das ist das Auto. Das andere ist, ich möchte das Verhalten so und so haben. |
| 01:25:12,360 | 01:25:18,000 | Robert | Das Ziel wäre natürlich, dass ich möglichst viel im Sharepad mache, aber auch dort muss |
| 01:25:18,000 | 01:25:19,020 | Robert | ich Dinge konfigurieren. |
| 01:25:20,990 | 01:25:26,820 | Robert | Also wie zum Beispiel der Benutzer soll eine Verwarnung bekommen, wenn er überzieht, keine |
| 01:25:27,060 | 01:25:27,180 | Robert | Ahnung. |
| 01:25:27,510 | 01:25:31,120 | Robert | Das können wir jetzt komplett im Sharepad machen unter Umständen, aber als Benutzersicht |
| 01:25:31,290 | 01:25:31,980 | Robert | ist das nicht relevant. |
| 01:25:32,660 | 01:25:38,119 | Marcel | Genau, aber wir müssen konfigurieren, dann müssen wir uns überlegen, was, das machen |
| 01:25:38,120 | 01:25:41,620 | Marcel | Was muss man konfigurieren können, was die ZK betrifft? |
| 01:25:43,700 | 01:25:49,540 | Marcel | Was die Ressource alleine betrifft, ohne ZK-Relevanz, interessiert die Schnittstelle nicht. |
| 01:25:50,080 | 01:25:51,760 | Marcel | Ist aber für die Use Cases durchaus wichtig. |
| 01:25:53,380 | 01:25:59,600 | Robert | Ich würde uns dort die Komplett-Use Cases anschauen und dann können wir schauen, was die ZK-Relevanz ist. |
| 01:26:00,120 | 01:26:09,820 | Robert | Grundsätzlich wird der Mai vorschlagen, dass man die komplette Konfiguration über das SharePad macht. |
| 01:26:10,050 | 01:26:10,660 | Robert | Ja gerne. |
| 01:26:10,710 | 01:26:23,600 | Robert | Also beim Programmieren der Firmware wäre die Seriennummer geladen und damit endet das Voreinstellen beim Zusammenbauen. |
| 01:26:24,170 | 01:26:24,400 | Marcel | Ja. |
| 01:26:27,080 | 01:26:35,400 | Marcel | Also, um da schon vorweg zu spoilern: Wir haben jetzt schon eine eindeutige Nummer, und zwar ist das die, glaube ich, dass wir die EMI des Modems verwenden. |
| 01:26:36,860 | 01:26:38,700 | Marcel | Ich muss nur kurz schauen, wo sitzt das Grafana? |
| 01:26:39,780 | 01:26:45,960 | Robert | Ja, das ist praktisch, weil die eindeutig ist. Also garantiert eindeutig, so meine ich. |
| 01:26:46,140 | 01:26:47,900 | Marcel | Ja, das war auch unser Gedanke. |
| 01:26:50,160 | 01:26:53,860 | Marcel | Lass mich kurz schauen: New Backend Overview |
| 01:26:54,520 | 01:26:57,860 | Robert | Und das ist auch deshalb praktisch, weil ich die Karte wechseln kann |
| 01:26:58,680 | 01:26:58,860 | Marcel | Ja |
| 01:26:58,940 | 01:26:59,960 | Robert | Also die SAM |
| 01:27:00,960 | 01:27:02,020 | Robert | Und das hat keinen Einfluss |
| 01:27:02,260 | 01:27:03,300 | Marcel | Genau, die E-Mail nutzen wir |
| 01:27:04,200 | 01:27:06,520 | Marcel | Ja, also wir nutzen die E-Mail für die eindeutige Zuordnung |
| 01:27:07,120 | 01:27:11,920 | Marcel | Die ist nämlich auch als scannbarer QR-Code auf dem Ding drauf |
| 01:27:12,000 | 01:27:13,680 | Robert | Ah, noch besser |
| 01:27:13,920 | 01:27:14,000 | Marcel | Ja |
| 01:27:14,960 | 01:27:18,100 | Marcel | Wobei man dazu natürlich das Gehäuse auch mal aufmachen muss, aber trotzdem ja |
| 01:27:18,260 | 01:27:29,920 | Robert | Ne, aber dass man eine Nummer hat, die eindeutig ist, die nicht an der SIM-Karte dranhängt, sondern wirklich am Modem und dann an der aufgedruckten oder was immer mehr. |
| 01:27:30,020 | 01:27:34,400 | Marcel | Technisch gesehen kann man das Modem bei den Dingern leider rausbauen, aber okay. |
| 01:27:37,280 | 01:27:38,200 | Robert | Wie kann ich das Modem rausbauen? |
| 01:27:38,840 | 01:27:41,540 | Marcel | Das Modem ist eine PCI-Steckkarte bei der neuen. |
| 01:27:42,900 | 01:27:43,640 | Robert | Ah, okay. |
| 01:27:44,140 | 01:27:45,160 | Marcel | Also die ist nicht fest. |
| 01:27:45,200 | 01:27:45,580 | Marcel | Ja, gut. |
| 01:27:45,960 | 01:27:46,920 | Marcel | Aber, ja, genau. |
| 01:27:47,500 | 01:27:49,300 | Marcel | Das gehört in die Kategorie, ja, |
| 01:27:50,420 | 01:27:51,380 | Marcel | unwahrscheinlich, dass es passiert. |
| 01:27:51,530 | 01:27:53,760 | Marcel | Und der, der das macht, der muss sich halt |
| 01:27:53,810 | 01:27:55,520 | Marcel | dessen bewusst sein. Genau, aber das ist |
| 01:27:55,660 | 01:27:57,600 | Marcel | ein Identifikator, den wir |
| 01:27:57,660 | 01:27:59,460 | Marcel | für uns intern schon in der jetzigen Datenbank |
| 01:27:59,560 | 01:28:01,680 | Marcel | drin haben. Also e-mail ist eine gute |
| 01:28:01,840 | 01:28:03,560 | Marcel | Nummer, weil die ist festvergeben, die muss ich |
| 01:28:03,560 | 01:28:05,660 | Marcel | nichts machen. Ich kann theoretisch mit dem Prozess |
| 01:28:05,820 | 01:28:07,700 | Marcel | überlegen, dass die nochmal außen drauf geschrieben wird |
| 01:28:07,800 | 01:28:08,300 | Marcel | im Buchstaben |
| 01:28:09,580 | 01:28:11,420 | Marcel | oder als Barcode außen drauf gekriegt wird, |
| 01:28:11,520 | 01:28:12,480 | Marcel | sobald das Ding beschraubt wurde. |
| 01:28:14,460 | 01:28:14,840 | Marcel | Okay, also. |
| 01:28:15,180 | 01:28:19,460 | Robert | Ja, das wäre tatsächlich nur so ein Punkt, dass man es vor außen sehen konnte. |
| 01:28:19,580 | 01:28:20,580 | Robert | Das stimmt, das wäre gut. |
| 01:28:22,020 | 01:28:23,160 | Robert | Okay, das können wir nächstes Mal besprechen. |
| 01:28:23,220 | 01:28:24,220 | Robert | Machen wir da nächstes Mal weiter. |
| 01:28:24,520 | 01:28:31,100 | Marcel | Genau, wir haben Registrieren und Konfigurieren von einem Auto, einer Ressource, |
| 01:28:32,880 | 01:28:34,460 | Marcel | in Kombination auch mit der ZK. |
| 01:28:35,620 | 01:28:40,400 | Robert | Das Thema Nutzeraufnahme. |
| 01:28:41,440 | 01:28:43,460 | Marcel | Mit Mitglied. Erstmal Mitgliedsaufnahme. |
| 01:28:43,980 | 01:28:44,960 | Marcel | Mitglieder anlegen. |
| 01:28:46,400 | 01:28:47,740 | Robert | Mitglieder anlegen gibt es ja schon, |
| 01:28:48,020 | 01:28:48,960 | Robert | den Use Case, aber... |
| 01:28:49,160 | 01:28:49,880 | Marcel | Ist auch ein Use Case. |
| 01:28:50,720 | 01:28:53,920 | Marcel | Mitglied ändern oder Daten überarbeiten ist ein Use Case. |
| 01:28:54,960 | 01:28:56,700 | Marcel | Wie, was, wo, was wird da gemacht. |
| 01:28:57,240 | 01:28:58,900 | Marcel | Und dann kommen wir zu den neuen Sachen eben, |
| 01:28:59,100 | 01:29:03,620 | Marcel | nämlich Nutzer anlegen und mit Mitgliedern verknüpfen. |
| 01:29:04,660 | 01:29:05,060 | Robert | Ja, genau. |
| 01:29:05,560 | 01:29:08,120 | Robert | Weil ich meine, Mitglieder sind ja jetzt aus ZK-Sicht, |
| 01:29:08,440 | 01:29:09,140 | Robert | gibt es die nicht mehr. |
| 01:29:10,280 | 01:29:11,080 | Marcel | Ja, ist richtig. |
| 01:29:12,480 | 01:29:15,500 | Robert | Insofern braucht man da nicht früh darüber reden. |
| 01:29:16,020 | 01:29:19,460 | Robert | Das Thema Mitglieder ist implementiert: Anlegen ändern, löschen und so weiter. |
| 01:29:20,120 | 01:29:21,900 | Robert | Als ZK-Sicht gibt es keine Mitglieder. |
| 01:29:22,340 | 01:29:28,120 | Robert | Darum denke ich, es geht nur um Nutzer anlegen, Mitglieder zuordnen. |
| 01:29:29,480 | 01:29:30,040 | Marcel | Genau. |
| 01:29:31,020 | 01:29:36,140 | Marcel | Und da ist aber spannend, da ist ein spannender Punkt, den wir dann im Rahmen dieses Nutzer |
| 01:29:36,320 | 01:29:41,620 | Marcel | anlegen und speziell auch Nutzer zuordnen betrachten müssen, ist: Wie läuft das ab? |
| 01:29:42,340 | 01:29:50,260 | Marcel | weil ich ja mit meinem Nutzer bei zwei Vereinen und bei einem Verein auch bei mehreren Mitgliedern registriert sein kann. |
| 01:29:51,900 | 01:29:56,800 | Marcel | Oder gibt es da eine Trennung und ich sage, nein, ein Nutzer gehört zu einem Verein |
| 01:29:58,180 | 01:30:02,140 | Marcel | und wenn der in einem anderen Verein Mitglied werden will, der natürliche echte Mensch, |
| 01:30:02,680 | 01:30:07,020 | Marcel | dann muss er zu denen gehen und kriegt von denen einen neuen Nutzer angelegt. |
| 01:30:09,220 | 01:30:11,640 | Marcel | Das ist ein konstruktiver Gedanke, den man sich machen muss. |
| 01:30:12,620 | 01:30:16,400 | Marcel | Der offene Gedanke ist natürlich, ich lege mir einen Nutzer an. |
| 01:30:17,610 | 01:30:23,200 | Marcel | Dieser Nutzer hat in dem Login von dir, kann ich mich einfach als Nutzer registrieren. |
| 01:30:23,210 | 01:30:27,380 | Marcel | Das ist aber nichts anderes, außer dass ich mich anmelden kann und ich sehe nur meine Nutzerdaten. |
| 01:30:27,550 | 01:30:27,640 | Marcel | Punkt. |
| 01:30:28,580 | 01:30:31,640 | Marcel | Und bei diesem Nutzerdaten, wenn ich die öffne, ist auch ein QR-Code. |
| 01:30:32,280 | 01:30:34,920 | Marcel | Und mit diesem QR-Code gehe ich dann zu meinem Verein und sage: Schaut mal her, ich habe |
| 01:30:34,980 | 01:30:36,920 | Marcel | mir einen Nutzer angelegt, so wie es in der Vorbereitung stand. |
| 01:30:37,150 | 01:30:38,960 | Marcel | Oder man macht das mit bei der Aufnahme. |
| 01:30:39,500 | 01:30:41,380 | Marcel | und dann wird dieser QR-Code von dem Verein gescannt |
| 01:30:41,480 | 01:30:43,020 | Marcel | und damit verheiratet man den Nutzer, |
| 01:30:43,200 | 01:30:44,200 | Marcel | dessen ID mit dem Verein. |
| 01:30:46,680 | 01:30:47,580 | Marcel | Und das kann ich mit beliebig |
| 01:30:47,700 | 01:30:49,560 | Marcel | vielen Vereinen machen und mit beliebig |
| 01:30:49,640 | 01:30:51,060 | Marcel | vielen Mitgliedern, dass ich darüber |
| 01:30:51,400 | 01:30:53,220 | Marcel | quasi einen Mitglied habe und dann verheiratet ich |
| 01:30:53,280 | 01:30:55,560 | Marcel | Verein-Mitglied, also Mitglied-Hankerben-Verein |
| 01:30:55,940 | 01:30:57,320 | Marcel | mit einem Nutzer und dieser Nutzer ist |
| 01:30:57,500 | 01:30:58,600 | Marcel | komplett neutral, unabhängig |
| 01:30:59,500 | 01:31:01,100 | Marcel | und kann sich auch selbst schon anlegen, bevor |
| 01:31:01,210 | 01:31:02,720 | Marcel | er überhaupt zum Aufnahmegespräch kommt. |
| 01:31:03,700 | 01:31:04,960 | Robert | Ja, das war mein Ding. |
| 01:31:06,700 | 01:31:07,800 | Robert | Ich habe pro Nutzer |
| 01:31:07,870 | 01:31:08,600 | Robert | eigentlich zwei |
| 01:31:08,880 | 01:31:10,180 | Robert | Tabellen. Ich habe die |
| 01:31:10,320 | 01:31:12,060 | Robert | jetzt ganz aus der |
| 01:31:12,140 | 01:31:13,720 | Robert | Sicht gesehen. Ich habe die einerseits |
| 01:31:15,270 | 01:31:15,840 | Robert | nur die |
| 01:31:19,420 | 01:31:20,680 | Robert | E-Mail-Adresse |
| 01:31:21,000 | 01:31:22,680 | Robert | und das Passwort |
| 01:31:22,860 | 01:31:24,260 | Robert | für den Login für den eigentlichen. |
| 01:31:25,400 | 01:31:26,460 | Marcel | Der hängt aber mehr dran. |
| 01:31:26,570 | 01:31:28,500 | Marcel | Also ich würde an die Nutzertabelle beinhaltet |
| 01:31:28,530 | 01:31:30,260 | Marcel | aus meiner Sicht alle Nutzerdaten. |
| 01:31:31,340 | 01:31:32,380 | Marcel | Das heißt, die |
| 01:31:32,980 | 01:31:34,420 | Marcel | natürlich Login-Name, |
| 01:31:34,890 | 01:31:36,120 | Marcel | also E-Mail-Adresse, |
| 01:31:37,060 | 01:31:37,460 | Marcel | Name, |
| 01:31:38,500 | 01:31:43,900 | Marcel | Passwort, Anschrift und die UID meines Chips. |
| 01:31:44,760 | 01:31:47,660 | Marcel | Bestimmte dieser Informationen kann ich nicht pflegen. |
| 01:31:47,940 | 01:31:50,860 | Marcel | Die kann nur ein Administrator pflegen, also zum Beispiel die UID meines Chips. |
| 01:31:52,760 | 01:31:55,080 | Marcel | Die ist für mich unsichtbar und die kann ich auch nicht befüllen. |
| 01:31:56,460 | 01:31:57,420 | Marcel | Das ist genau das Ding. |
| 01:31:57,980 | 01:32:01,460 | Robert | Die kann nur der Administrator pro Verein pflegen. |
| 01:32:01,540 | 01:32:04,500 | Robert | ein anderer Verein will die eventuell nicht von |
| 01:32:05,820 | 01:32:06,620 | Robert | also Verein X |
| 01:32:06,880 | 01:32:08,200 | Robert | ich bin bei Verein X und Y |
| 01:32:09,480 | 01:32:11,320 | Robert | und der Admin-Verein von X |
| 01:32:11,620 | 01:32:13,300 | Robert | der Administrator von Verein X |
| 01:32:13,840 | 01:32:16,020 | Robert | will nicht, dass der Administrator |
| 01:32:16,100 | 01:32:16,780 | Robert | von Verein Y |
| 01:32:18,420 | 01:32:19,820 | Robert | ihm die Daten überschreibt |
| 01:32:20,860 | 01:32:21,980 | Robert | aber lass uns das nächste Mal |
| 01:32:22,080 | 01:32:23,540 | Marcel | ja, vollkommen richtig, also wir haben |
| 01:32:23,740 | 01:32:25,640 | Marcel | Nutzer, Nutzer zu einem |
| 01:32:25,880 | 01:32:27,360 | Marcel | Mitglied oder Verein hinzufügen |
| 01:32:28,420 | 01:32:29,719 | Marcel | und Nutzer und da |
| 01:32:29,680 | 01:32:31,080 | Marcel | relevante Daten ändern. |
| 01:32:31,270 | 01:32:33,480 | Marcel | Das ist ein anderer Use Case. Also wie ändere ich die Daten? |
| 01:32:33,490 | 01:32:35,700 | Marcel | Wie du gerade in deinem Beispiel schon gesagt hast, ist das nämlich durchaus |
| 01:32:35,770 | 01:32:37,560 | Marcel | ein anderer Anwendungsfall, |
| 01:32:37,560 | 01:32:39,140 | Marcel | die Daten zu ändern eines Nutzers. |
| 01:32:41,800 | 01:32:42,540 | Marcel | Wir haben den |
| 01:32:43,340 | 01:32:43,780 | Marcel | Fall |
| 01:32:44,880 | 01:32:46,140 | Marcel | Codieren und Aufbringen |
| 01:32:46,310 | 01:32:46,900 | Marcel | des Chips |
| 01:32:47,980 | 01:32:49,660 | Marcel | auf den Führerschein in Kombination mit |
| 01:32:49,780 | 01:32:50,540 | Marcel | Führerscheinkontrolle. |
| 01:32:54,300 | 01:32:55,600 | Marcel | Und was haben wir noch |
| 01:32:55,800 | 01:32:56,240 | Marcel | beim Nutzer? |
| 01:32:58,060 | 01:32:59,640 | Marcel | "Löschen des Nutzerkontos" |
| 01:33:01,580 | 01:33:05,280 | Marcel | Muss ein Usecast sein, wie löscht man ein Nutzerkonto, wie löscht man ein Mitglied? |
| 01:33:05,440 | 01:33:11,080 | Robert | Wir haben noch gar nicht gesprochen über das Thema Nutzer jetzt oder haben wir noch was bei den Nutzer? |
| 01:33:12,920 | 01:33:16,820 | Marcel | Wir haben grob durchdacht: Anlegen, Ändern, Löschen |
| 01:33:21,820 | 01:33:24,640 | Marcel | und Datenpflege von Fällen, also Berechtigungen. |
| 01:33:24,760 | 01:33:30,600 | Marcel | Wobei die Berechtigungen hier wiederum in der Kombination aus Nutzer und Mitglied entstehen. |
| 01:33:30,720 | 01:33:32,520 | Marcel | Oder sogar direkt am Mitglied hängen. Wahrscheinlich eher. |
| 01:33:34,600 | 01:33:36,520 | Robert | Ja, vielleicht ist es genau. |
| 01:33:37,940 | 01:33:40,840 | Robert | Dass ich einen Nutzer zum Mitglied dazufüge, ist das eine. |
| 01:33:41,280 | 01:33:42,380 | Robert | Also dazufügen und rausnehmen. |
| 01:33:43,840 | 01:33:47,540 | Robert | Ist auch die Frage, ob das zum Beispiel der Nutzer nicht selber machen kann. |
| 01:33:48,620 | 01:33:49,540 | Robert | Entschuldigung, das Mitglied. |
| 01:33:50,600 | 01:33:52,700 | Robert | Dass ich als Mitglied einen Nutzer einfach dazufüge. |
| 01:33:54,120 | 01:33:56,400 | Marcel | Muss man durchdenken, ist natürlich schön. |
| 01:33:57,480 | 01:34:02,280 | Marcel | Hat aber den Nachteil, dass du ja trotzdem, also du musst es einmal über den Verein machen, alleine schon wegen der Führerscheinkontrolle. |
| 01:34:02,840 | 01:34:03,040 | Marcel | Genau. |
| 01:34:03,200 | 01:34:03,540 | Marcel | Bekleben. |
| 01:34:03,600 | 01:34:03,700 | Marcel | Genau. |
| 01:34:04,140 | 01:34:15,900 | Marcel | Aber ihn hinzufügen, ja, wer darf ihn hinzufügen, dann brauchst du wieder ein rechtes Konzept, weil nicht jeder Nutzer eines Mitglieds darf andere hinzufügen. |
| 01:34:16,900 | 01:34:17,060 | Robert | Ja. |
| 01:34:18,800 | 01:34:21,040 | Robert | Das müsst ihr uns genau machen, lasst uns das am nächsten Mal. |
| 01:34:21,290 | 01:34:21,380 | Robert | Ja. |
| 01:34:21,380 | 01:34:22,340 | Robert | Aber das ist auf jeden Fall ein Thema, ja. |
| 01:34:23,100 | 01:34:24,760 | Robert | Und dann die Kilometer-Erfassung. |
| 01:34:27,080 | 01:34:29,000 | Robert | Die müssen wir uns auch noch mit durchdringen. |
| 01:34:29,140 | 01:34:29,280 | Robert | Genau. |
| 01:34:29,900 | 01:34:33,780 | Robert | Also wie werden die Kilometer erfasst und wie werden sie übermittelt? |
| 01:34:34,460 | 01:34:36,340 | Robert | Also wie sie erfasst werden ist aber nicht die Frage. |
| 01:34:36,360 | 01:34:41,400 | Robert | Aber du hast jetzt schon skizziert, dass man eventuell in Paketen die Systeme überlegt. |
| 01:34:42,740 | 01:34:45,440 | Robert | Dass die ZK automatisch... |
| 01:34:45,980 | 01:34:48,620 | Marcel | Lass uns das vielleicht noch mal kurz anstürt zurückschrauben. |
| 01:34:48,760 | 01:34:53,060 | Marcel | Wir müssen zwei Informationen in dem System verarbeitet werden. |
| 01:34:53,060 | 01:34:57,100 | Marcel | Das eine sind die Buchungszeiten zu den Ressourcen. |
| 01:34:58,440 | 01:35:01,380 | Marcel | Die kommen aus deinem System, wenn in deinem System verarbeitet sind, da. |
| 01:35:02,720 | 01:35:05,220 | Marcel | Und dann kommen noch die Informationen der Kilometer dazu. |
| 01:35:05,860 | 01:35:09,540 | Marcel | Und die Informationen der Kilometer können auf zwei Wegen ins System kommen. |
| 01:35:10,440 | 01:35:15,620 | Marcel | Entweder über manuelle Eingabe, dass ich sage, ich habe eine Buchung und ich trage per Hand dazu die Kilometer ein. |
| 01:35:16,340 | 01:35:20,420 | Marcel | Es wird zwar eine geben, die das so wollen und weiter so machen, weil es einfacher ist. |
| 01:35:21,100 | 01:35:27,000 | Marcel | Und es gibt die Möglichkeit, dass die Kilometerinformationen über eine technische Schnittstelle reinkommen. |
| 01:35:27,900 | 01:35:29,100 | Marcel | Zum Beispiel die ZK. |
| 01:35:33,220 | 01:35:35,620 | Marcel | Erstmal nur die ZK, würde ich sagen, als technische Schnittstelle. |
| 01:35:36,460 | 01:35:42,380 | Marcel | Und die ZK liefert Informationen zu gefahrenen Kilometern an Sharepad. |
| 01:35:44,040 | 01:35:53,120 | Marcel | Aber kann eigentlich nur die Information liefern, von Uhrzeit bis Uhrzeit und Datum wurden so und so viele Kilometer gefahren. |
| 01:35:55,500 | 01:35:58,720 | Marcel | Mehr halte ich als schwierig, eine Information zu geben. |
| 01:35:58,760 | 01:36:03,200 | Marcel | Aber das ist eben auch eine Information, das ist ja von der Use Case, wie kommen Kilometer ins System? |
| 01:36:03,660 | 01:36:15,220 | Marcel | Entweder auf einem automatischen Weg über eine Schnittstelle von dem Auto direkt oder auf einem manuellen Weg durch Eingabe dann basierend auf den Buchungsdaten. |
| 01:36:16,660 | 01:36:20,060 | Robert | Also mir geht es jetzt schon um den automatischen Weg. |
| 01:36:21,280 | 01:36:28,380 | Robert | Und dort ist es ja so, dass unter Umständen das System auch in der Lage sei, |
| 01:36:28,740 | 01:36:31,000 | Robert | ohne ZK-Kilometer zu erfassen. |
| 01:36:33,080 | 01:36:37,140 | Robert | Beziehungsweise, wenn man davon ausgeht, dass die ZK möglichst dumm bleibt, |
| 01:36:37,980 | 01:36:48,900 | Robert | Soll sie generell so funktionieren, dass sie auch ohne großartig die Reservierungen zu interpretieren, |
| 01:36:50,120 | 01:36:55,600 | Robert | die Kilometer einfach erfasst und dann mich schickt und ich mache dann das Zusammenstitchen. |
| 01:36:56,040 | 01:36:57,560 | Marcel | Genau, das ist mein Konzept. |
| 01:36:58,180 | 01:37:04,320 | Marcel | Das ist absolut mein Konzept, weil damit ist die Kilometererfassung unabhängig von Reservierungen und Auf- und Zusperren und irgendwas. |
| 01:37:05,320 | 01:37:11,800 | Marcel | Auf der einen Seite unabhängig davon, weil ich sage, ich muss die Daten nicht in der ZK merchen, zumal sie oft auch nicht zusammenhängen. |
| 01:37:12,760 | 01:37:19,080 | Marcel | Und zweitens, was es vielleicht auch Vereine gibt, die gar nicht die Zugangskontrolle als Zugangskontrolle mit Auf- und Zusperren nutzen wollen. |
| 01:37:19,080 | 01:37:27,380 | Marcel | Und das sind beides Gründe dafür, dass ich sage, die Kilometerfassung ist die Kilometerfassung, die basiert nur auf den GPS-Daten und Änderung der GPS-Daten. |
| 01:37:27,380 | 01:37:33,320 | Marcel | Und die Zugangskontrolle mit Auf- und Zusperren hängt an Terminbuchungen und Karten, IDs und so weiter und so fort. |
| 01:37:35,460 | 01:37:37,740 | Robert | Das würde auf jeden Fall noch mit dem nächsten Mal durchgehen. |
| 01:37:37,900 | 01:37:38,940 | Marcel | Genau, machen wir gerne. |
| 01:37:41,580 | 01:37:43,380 | Marcel | Große, großes Topic für nächstes Mal. |
| 01:37:44,880 | 01:37:46,520 | Marcel | Ein Prozessdiagramm, fast schon nicht nur. |
| 01:37:46,740 | 01:37:49,940 | Marcel | Ein Use Case ist dann das Thema Kilometererfassung und Übertragung. |
| 01:37:50,320 | 01:37:51,060 | Marcel | Wie geht das? |
| 01:37:53,160 | 01:37:54,880 | Marcel | Gut, was haben wir noch für Use Cases? |
| 01:37:54,940 | 01:37:58,920 | Marcel | Wir sind jetzt ein bisschen abgeschreift. Wir hatten User und Mitglieder, glaube ich, ganz gut beleuchtet. |
| 01:37:59,060 | 01:38:01,560 | Marcel | Was wir noch an Use Cases brauchen oder haben werden. |
| 01:38:01,580 | 01:38:07,680 | Marcel | Wir haben administrativ angefangen, haben gesagt, Karte löschen, Mitglied löschen, Nutzer löschen. |
| 01:38:09,360 | 01:38:12,680 | Marcel | Das sind so typische administrative Sachen oder auch das Ganze eben anlegen. |
| 01:38:14,520 | 01:38:16,780 | Marcel | Wir hatten, also auch Karte anlegen muss ja funktionieren. |
| 01:38:19,040 | 01:38:22,800 | Marcel | Da ist dann das Thema Karte kodieren auch mit drin, weil ich würde zumindest bei dem bleiben, |
| 01:38:22,940 | 01:38:28,660 | Marcel | dass man mit einem Schlüssel kodiert und die UID nochmal in Sechs Sektor 1 schreibt. |
| 01:38:31,160 | 01:38:32,860 | Marcel | Dann haben wir Karten kodieren. |
| 01:38:33,000 | 01:38:33,560 | Marcel | Was haben wir noch? |
| 01:38:34,560 | 01:38:36,600 | Marcel | Wir haben das Thema Berechtigungen. |
| 01:38:37,460 | 01:38:38,340 | Marcel | Berechtigungen vergeben. |
| 01:38:39,320 | 01:38:40,360 | Marcel | Also wer darf |
| 01:38:42,000 | 01:38:43,200 | Marcel | was machen in einem System? |
| 01:38:43,840 | 01:38:44,400 | Marcel | Diese Rollen. |
| 01:38:44,870 | 01:38:45,760 | Marcel | Das muss man noch definieren. |
| 01:38:45,760 | 01:38:47,340 | Marcel | Wie ist da der Anwendungsfall? |
| 01:38:47,540 | 01:38:49,280 | Marcel | Wie ist das Vorgehen? |
| 01:38:49,700 | 01:38:51,460 | Marcel | Damit ich einen Benutzer zu einem |
| 01:38:51,880 | 01:38:52,840 | Marcel | Drei-Stufen-Netz, glaube ich, |
| 01:38:53,400 | 01:38:55,440 | Marcel | Anwender, Super-User und Administrator |
| 01:38:55,510 | 01:38:55,960 | Marcel | oder sowas, |
| 01:38:57,700 | 01:38:59,100 | Marcel | wie kann ich diese Rollen vergeben? |
| 01:38:59,220 | 01:39:00,020 | Marcel | Wer darf die vergeben? |
| 01:39:05,740 | 01:39:14,520 | Marcel | Gut, die anderen Use Cases hast du bei dir drin, die betreffen jetzt weniger, also wie lege ich einen Verein an den neuen und so was, das heißt, du hast ja auch alles schon definiert viel und ist auch hier nicht relevant. |
| 01:39:14,780 | 01:39:27,120 | Marcel | Wir hatten Ressourcen anlegen, wir hatten, klar, Ressourcen ändern, also den Standort ändern, |
| 01:39:28,540 | 01:39:34,940 | Marcel | wo steht die Ressource, vielleicht abrechnungsrelevante Informationen ändern, wie Stunden oder Kilometersätze |
| 01:39:34,940 | 01:39:35,880 | Marcel | von einer Ressource. |
| 01:39:36,960 | 01:39:41,580 | Robert | Ja, prinzipiell ja, wobei das ja auch kontrarezeptar relevant ist. |
| 01:39:42,000 | 01:39:46,920 | Marcel | Nein, nein, aber wir haben gesagt, du hast ja gemeint, richtigerweise, und das finde ich gut, dass wir gesamt über Use Cases mal reden. |
| 01:39:47,020 | 01:39:52,100 | Marcel | Wir müssen die anderen ja auch nur grob ansprechen, weil dann haben wir eine Liste, die wir über mehrere Termine ausarbeiten, |
| 01:39:52,360 | 01:39:58,680 | Marcel | und wo wir dann eine Liste an Use Cases dem Klaus geben können und sagen, du schau mal durch, fällt dir noch was auf an Use Cases? |
| 01:39:58,960 | 01:40:02,660 | Marcel | Lest dir die mal durch und lass uns einen Termin machen, was wir in Use Cases vergessen haben. |
| 01:40:05,440 | 01:40:11,780 | Marcel | Genau, Daten einlesen von einer externen Software. |
| 01:40:12,240 | 01:40:14,220 | Marcel | Also was Kilometer angeht, da hast du ja schon was gebaut. |
| 01:40:14,320 | 01:40:18,660 | Marcel | Also sprich Kilometerdaten zum Beispiel einlesen, die kommen ja momentan glaube ich aus Access. |
| 01:40:19,960 | 01:40:20,480 | Marcel | In dein System. |
| 01:40:21,100 | 01:40:23,740 | Marcel | Oder andersrum, nein, du gibst Export von Buchungsdaten. |
| 01:40:23,740 | 01:40:24,660 | Marcel | So rum war das aktuell, stimmt. |
| 01:40:24,980 | 01:40:27,280 | Robert | Genau, und dann gibst du den Access die Kilometer ein. |
| 01:40:27,760 | 01:40:27,820 | Marcel | Ja. |
| 01:40:28,880 | 01:40:31,700 | Robert | Was sich noch gerne mit aufnehmen wird, wäre Fernware Update. |
| 01:40:32,720 | 01:40:33,940 | Marcel | Oh ja, genau, klar. |
| 01:40:34,180 | 01:40:39,600 | Marcel | Firmware-Update, also Firmware-Versionsverwaltung an Abhängigkeit der Ressourcen. |
| 01:40:40,520 | 01:40:40,820 | Robert | Genau. |
| 01:40:41,540 | 01:40:44,200 | Marcel | Weil nur das reine Update ist ja nur die halbe Miete. |
| 01:40:45,180 | 01:40:46,000 | Robert | Ja, genau. |
| 01:40:47,880 | 01:40:51,700 | Marcel | Wir haben auf jeden Fall noch das Thema Protokolle, ZK-Protokolle, |
| 01:40:51,980 | 01:40:53,960 | Marcel | hochladen, sichten und anschauen. |
| 01:40:55,440 | 01:41:00,460 | Marcel | Und da eventuell sogar noch den Fall für später, ganz definitiv Backlog-Thema, |
| 01:41:00,820 | 01:41:09,540 | Marcel | aber Warnings ausgeben, also ein ZK hat seit 2 Tagen keine Protokolle mehr hochgeladen |
| 01:41:10,760 | 01:41:15,100 | Marcel | oder ein ZK hat ein Protokoll hochgeladen, ein NFC Board nicht gefunden |
| 01:41:16,739 | 01:41:19,020 | Marcel | Ist das kein Problem? Ist das kein Problem? |
| 01:41:20,800 | 01:41:29,800 | Marcel | Dass man da Warnings und E-Mails und entsprechend mit Tasks die Administratoren warnt, zu denen die Ressource gehört, dass da was nicht passt |
| 01:41:33,140 | 01:41:35,000 | Marcel | Dann, was haben wir noch? |
| 01:41:42,680 | 01:41:44,300 | Marcel | Auswertung haben wir gesagt, erstmal gar nicht. |
| 01:41:49,180 | 01:41:50,480 | Robert | Das ist mir für nichts mehr dann. |
| 01:41:53,140 | 01:41:55,340 | Marcel | Ich gehe gerade mal bei uns interne Prozesse durch. |
| 01:41:56,360 | 01:41:57,920 | Marcel | Ressourcen stilllegen wäre noch so ein Thema. |
| 01:41:57,940 | 01:42:11,700 | Marcel | Also Autoverkauf. Aber das ist auch wieder der klassische Fall wie überall. Der klassische Lifecycle, der abgebildet worden muss beim Mitglied, beim Nutzer, beim Ressource. |
| 01:42:12,160 | 01:42:19,980 | Marcel | Heißt anlegen, modifizieren mit den entsprechenden Parametern, die noch zu definieren sind und stilllegen, Schrägstrich löschen. |
| 01:42:20,840 | 01:42:22,540 | Marcel | weil bei beiden, sowohl beim Mitglied |
| 01:42:22,550 | 01:42:24,060 | Marcel | als auch beim Nutzer, als auch bei der Ressource |
| 01:42:24,150 | 01:42:26,220 | Marcel | kann ich sie nicht gleich löschen, sondern ich kann sie nur stilllegen |
| 01:42:26,720 | 01:42:28,580 | Marcel | und kann sie dann nach einer adäquaten Zeit |
| 01:42:28,800 | 01:42:30,500 | Marcel | wenn alle abrechnungsrelevanten |
| 01:42:31,120 | 01:42:32,400 | Marcel | Themen durch sind, dann kann ich die Daten |
| 01:42:32,450 | 01:42:32,820 | Marcel | erst löschen |
| 01:42:34,720 | 01:42:35,020 | Marcel | Genau |
| 01:42:36,600 | 01:42:37,260 | Marcel | Ne, sonst |
| 01:42:38,720 | 01:42:40,480 | Marcel | fällt mir jetzt auch nichts an, aber vielleicht kommt noch was |
| 01:42:42,820 | 01:42:43,180 | Robert | Passt |
| 01:42:44,420 | 01:42:44,780 | Robert | Gut |
| 01:42:45,760 | 01:42:46,000 | Robert | Dann |
| 01:42:47,250 | 01:42:48,620 | Robert | wollen wir einen Termin? |
| 01:42:49,110 | 01:42:50,020 | Marcel | Genau, das machen wir noch |
| 01:43:13,560 | 01:43:15,760 | Robert | Eher früh bis später |
| 01:43:20,800 | 01:43:23,620 | Marcel | Dann lass uns die Woche 13. bis 16. mal anschauen. |
| 01:43:24,960 | 01:43:28,740 | Marcel | Da würde bei mir der... |
| 01:43:32,480 | 01:43:36,980 | Marcel | Ja, theoretisch Montag, Mittwoch, Donnerstag würden bei mir gehen. |
| 01:43:47,219 | 01:43:49,580 | Robert | Okay, dann soll man Montag machen. |
| 01:43:50,060 | 01:43:50,120 | Robert | ja |
| 01:44:21,380 | 01:44:21,540 | Robert | Gut. |
| 01:44:23,640 | 01:44:24,600 | Speaker 3 | Fast Montag 13. |
| 01:44:25,660 | 01:44:26,980 | Speaker 3 | Ist eingetragen. |
| 01:44:30,300 | 01:44:32,400 | Speaker 3 | Gut, dann stoppe ich mal. |
