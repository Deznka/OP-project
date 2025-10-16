#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <limits>

using namespace std;

int main() {
    ifstream strumienPlikuWejsciowego("bayg29.txt");

    if (!strumienPlikuWejsciowego.is_open()) {
        cout << "Blad otwarcia pliku!" << endl;
        return 1;
    }

    int liczbaMiast = 0;
    strumienPlikuWejsciowego >> liczbaMiast;

    string liniaTekstu;
    getline(strumienPlikuWejsciowego, liniaTekstu);

    vector<pair<int, int>> punktyMiast(liczbaMiast + 1);

    while (getline(strumienPlikuWejsciowego, liniaTekstu)) {
        stringstream strumienLinii(liniaTekstu);
        int identyfikator;
        int wspolrzednaX;
        int wspolrzednaY;
        if (strumienLinii >> identyfikator >> wspolrzednaX >> wspolrzednaY) {
            punktyMiast[identyfikator] = {wspolrzednaX, wspolrzednaY};
        }
    }

    strumienPlikuWejsciowego.close();

    vector<vector<int>> macierzOdleglosci(liczbaMiast + 1, vector<int>(liczbaMiast + 1, 0));

    for (int i = 1; i <= liczbaMiast; i++) {
        for (int j = 1; j <= liczbaMiast; j++) {
            if (i != j) {
                int x1 = punktyMiast[i].first;
                int y1 = punktyMiast[i].second;
                int x2 = punktyMiast[j].first;
                int y2 = punktyMiast[j].second;
                double odleglosc = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
                macierzOdleglosci[i][j] = round(odleglosc);
            }
        }
    }

    int miastoStartowe = 1;
    vector<bool> miastaOdwiedzone(liczbaMiast + 1, false);
    vector<int> trasaPodrozy;
    trasaPodrozy.push_back(miastoStartowe);
    miastaOdwiedzone[miastoStartowe] = true;
    int aktualneMiasto = miastoStartowe;

    for (int i = 0; i < liczbaMiast - 1; i++) {
        int najlepszeMiasto = -1;
        int najmniejszaOdleglosc = numeric_limits<int>::max();

        for (int kandydat = 1; kandydat <= liczbaMiast; kandydat++) {
            if (!miastaOdwiedzone[kandydat]) {
                int odleglosc = macierzOdleglosci[aktualneMiasto][kandydat];
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

    int kosztTrasy = 0;

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