#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <ctime>

using namespace std;

void generujMiasta(int liczbaMiast, const string& nazwaPliku) {
    ofstream plikWyjsciowy(nazwaPliku);

    if (!plikWyjsciowy.is_open()) {
        cout << "Blad tworzenia pliku!" << endl;
        return;
    }

    srand(time(0));

    plikWyjsciowy << liczbaMiast << endl;

    for (int i = 1; i <= liczbaMiast; i++) {
        int x = rand() % 2000 + 1;
        int y = rand() % 2500 + 1;
        plikWyjsciowy << i << " " << x << " " << y << endl;
    }

    plikWyjsciowy.close();
    cout << "Wygenerowano " << liczbaMiast << " miast i zapisano do pliku " << nazwaPliku << endl;
}

int main() {
    cout << "=== PROBLEM KOMIWOJAZERA ===" << endl << endl;
    cout << "Wybierz opcje:" << endl;
    cout << "1. Wygeneruj nowe dane" << endl;
    cout << "2. Uzyj istniejacych danych z pliku test.txt" << endl;
    cout << "Wybor: ";

    int wybor;
    cin >> wybor;

    string nazwaPliku;

    if (wybor == 1) {
        int liczbaMiastDoWygenerowania;
        cout << "Podaj liczbe miast do wygenerowania: ";
        cin >> liczbaMiastDoWygenerowania;

        nazwaPliku = "generator.txt";
        generujMiasta(liczbaMiastDoWygenerowania, nazwaPliku);
        cout << endl;
    } else if (wybor == 2) {
        // TODO: Do podmiany nazwa
        nazwaPliku = "test.txt";
    } else {
        cout << "Nieprawidlowy wybor!" << endl;
        return 1;
    }

    ifstream strumienPlikuWejsciowego(nazwaPliku);

    if (!strumienPlikuWejsciowego.is_open()) {
        cout << "Blad otwarcia pliku!" << endl;
        return 1;
    }

    int liczbaMiast = 0;
    strumienPlikuWejsciowego >> liczbaMiast;

    string liniaTekstu;
    getline(strumienPlikuWejsciowego, liniaTekstu);

    vector<pair<double, double>> punktyMiast(liczbaMiast + 1);

    while (getline(strumienPlikuWejsciowego, liniaTekstu)) {
        stringstream strumienLinii(liniaTekstu);
        int identyfikator;
        double wspolrzednaX;
        double wspolrzednaY;
        if (strumienLinii >> identyfikator >> wspolrzednaX >> wspolrzednaY) {
            punktyMiast[identyfikator] = {wspolrzednaX, wspolrzednaY};
        }
    }

    strumienPlikuWejsciowego.close();

    vector<vector<double>> macierzOdleglosci(liczbaMiast + 1, vector<double>(liczbaMiast + 1, 0));

    for (int i = 1; i <= liczbaMiast; i++) {
        for (int j = 1; j <= liczbaMiast; j++) {
            if (i != j) {
                double x1 = punktyMiast[i].first;
                double y1 = punktyMiast[i].second;
                double x2 = punktyMiast[j].first;
                double y2 = punktyMiast[j].second;
                double odleglosc = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
                macierzOdleglosci[i][j] = odleglosc;
            }
        }
    }

    int miastoStartowe = 1;
    vector<bool> miastaOdwiedzone(liczbaMiast + 1, false);
    vector<double> trasaPodrozy;
    trasaPodrozy.push_back(miastoStartowe);
    miastaOdwiedzone[miastoStartowe] = true;
    int aktualneMiasto = miastoStartowe;

    for (int i = 0; i < liczbaMiast - 1; i++) {
        int najlepszeMiasto = -1;
        double najmniejszaOdleglosc = numeric_limits<double>::max();

        for (int kandydat = 1; kandydat <= liczbaMiast; kandydat++) {
            if (!miastaOdwiedzone[kandydat]) {
                double odleglosc = macierzOdleglosci[aktualneMiasto][kandydat];
                if (odleglosc < najmniejszaOdleglosc ||
                    (odleglosc == najmniejszaOdleglosc && (najlepszeMiasto == -1 || kandydat < najlepszeMiasto))) {
                    najmniejszaOdleglosc = odleglosc;
                    najlepszeMiasto = kandydat;
                }
            }
        }

        trasaPodrozy.push_back(najlepszeMiasto);
        miastaOdwiedzone[najlepszeMiasto] = true;
        aktualneMiasto = najlepszeMiasto;
    }

    double kosztTrasy = 0;

    for (size_t i = 0; i < trasaPodrozy.size() - 1; i++) {
        kosztTrasy += macierzOdleglosci[trasaPodrozy[i]][trasaPodrozy[i + 1]];
    }

    kosztTrasy += macierzOdleglosci[trasaPodrozy.back()][trasaPodrozy.front()];

    cout << "Liczba miast: " << liczbaMiast << endl << endl;
    cout << "Algorytm: Nearest Neighbor (zachlanny), start = 1" << endl;
    cout << "Trasa: ";
    for (size_t i = 0; i < trasaPodrozy.size(); i++) {
        cout << trasaPodrozy[i];
        if (i + 1 < trasaPodrozy.size()) {
            cout << " ";
        }
    }
    cout << endl;
    cout << "Koszt (z powrotem do startu): " << kosztTrasy << endl;

    return 0;
}