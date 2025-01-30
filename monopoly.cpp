#include <random>
#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
using namespace std;

class Player; // Déclaration anticipée

class Property {
  public:
    Player* owner; // Pointeur vers le propriétaire
    string name;
    int price;
    int houses;
    bool hotel;
    bool mortgaged;

  public:
    Property(string name, int price):
      name(name), price(price), owner(nullptr), houses(0), hotel(false), mortgaged(false)
    {}

    // Retourne le loyer en fonction des améliorations
    int getRent() const {
      if (mortgaged) return 0; // Pas de loyer si hypothéqué
      if (hotel) return price * 2; // Exemple : loyer avec hôtel
      return price / 10 + (houses * 50); // Exemple : loyer avec maisons
    }

};

class Plateau {
  public:
    vector<Property> properties;

    Plateau() {
      properties = {
        {"depart", 0}, {"boulevardDeBelleville", 60}, {"caisseCommunaute", 0}, {"rueLecourbe", 60}, {"impots", -200}, 
        {"gareMontparnasse", 200}, {"rueDeVaugirard", 100}, {"chance", 0}, {"rueDeCourcelles", 100}, {"avenueDeLaRepublique", 120},

        {"prison", 0}, {"boulevardDeLaVillette", 140}, {"electricite", 150}, {"avenueDeNeuilly", 140}, {"rueDeParadis", 160}, 
        {"gareDeLyon", 200}, {"avenueMozart", 180}, {"caisseCommunaute", 0}, {"boulevardST-Michel", 180}, {"placePigalle", 200},

        {"parc", 0}, {"avenueMatignon", 220}, {"chance", 0}, {"boullevradMalesherbes", 220}, {"avenueHenri-Martin", 240}, 
        {"gareDuNord", 200}, {"foubourgST-Honore", 260}, {"placeDeLaBourse", 260}, {"eaux", 150}, {"rueLaFayette", 280},

        {"allezPrison", 0}, {"avenueDeBreteuil", 300}, {"avenueFoch", 300}, {"caisseCommunaute", 0}, {"boulevardDesCapucines", 320}, 
        {"gareST-Lazare", 200}, {"chance", 0}, {"avenueDesChamps-Elysee", 350}, {"taxe", -200}, {"rueDeLaPaix", 400}
      };
    }

    // Nom de la case à la position donnée du plateau
    Property* getProperty(int position) {
      return &properties[position];
    }

    // Simule le lancer de deux dés à six faces
    pair<int, int> rollDice() const {
        random_device rd;
        uniform_int_distribution<int> dist(1, 6);
        return {dist(rd), dist(rd)};
    }

};

class Player {
  public:
    Plateau plateau;
    string name;                           // Nom du joueur
    int money;                             // Argent disponible
    int position;                          // Position actuelle sur le plateau
    int consecutiveDouble = 0;             // Nombre de doubles consécutifs
    int dice1, dice2, moveSteps;           // Valeurs des dés et le nombre de déplacements
    vector<Property*> ownedProperties;     // Liste des propriétés possédées

  public:
    /** Constructeur de la classe Player.
     * Initialise le nom, l'argent et la position du joueur.
     * @param namePlayer Le nom du joueur. 
    */
    Player(string namePlayer) {
      name = namePlayer;
      money = 1500;
      position = 0; // Position de départ
    }

    /** Retourne le nom du joueur.
     * @return 'name'.
     */
    string getName() {
      return name;
    }

    /** @return `true` si le joueur a lancé un double, sinon @return `false`.
     * Réinitialise le compteur de doubles consécutifs si ce n'est pas le cas.
     */
    bool isDouble() {
      if (dice1 != dice2) {
        consecutiveDouble=0;
        return false;
      } else {
        cout << "Vous avez fait un double, vous pouvez rejouer." << endl; 
        consecutiveDouble++;
        return true;
      }
    }

    /** Affiche la liste des propriétés possédées et l'argent disponible.
     * Indique si le joueur ne possède pas encore de propriété.
     */
    void displayOwnedProperties() {
      cout << "Vous avez " << money << " skibidibucks.";
      if (ownedProperties.empty()) {
        cout << " Vous ne possédez pas de propriétés." << endl;
      } else {
        cout << " Vous possédez les propriétés suivantes : ";
        for (size_t i = 0; i < ownedProperties.size(); i++) {
          cout << ownedProperties[i]->name;
          if (i != ownedProperties.size() - 1) {
            cout << ", "; // Ajouter une virgule entre les propriétés sauf pour la dernière
          }
        }
        cout << "." << endl;
      }
    }

    /** Effectue une tentative d'achat de la case actuelle.
     * Déduit le prix de la case de l'argent disponible si l'achat est confirmé.
     */
    void attemptPurchase(Property* currentProperty) {
      if (currentProperty->owner != nullptr) {
        cout << "Cette propriété appartient à " << currentProperty->owner->getName() << ". Vous devez payer un loyer de " << currentProperty->getRent() << "." << endl;
        money -= currentProperty->getRent();
        currentProperty->owner->money += currentProperty->getRent();
        return;
      } else if (money < currentProperty->price) {
        cout << "Vous n'avez pas assez d'argent pour acheter cette propriété." << endl;
        return;
      } else if (currentProperty->price > 0){
          string local_input;
          cout << "voulez-vous acheter cette propriete (O/N)?" << endl << ">>> ";
          cin >> local_input;

          if (local_input == "O" || local_input == "o") {
          currentProperty->owner = this; // Définir le propriétaire
          ownedProperties.push_back(currentProperty); // Ajouter au joueur
          money -= currentProperty->price;
          cout << name << " a acheté " << currentProperty->name << " pour " << currentProperty->price << " skibidibucks." << endl;
          }
      }
    }

    /** Simule le tour de jeu d'un joueur.
     * Lancer les dés, déplacer le joueur et gérer les interactions avec la case.
     */
    void play() {
      auto [dice1, dice2] = plateau.rollDice();
      cout << "Dé 1 : " << dice1 << "   |   " << "Dé 2 : " << dice2 << "   |   " << "nombre de déplacements : " << dice1 + dice2 << endl;
      position = (position + dice1 + dice2) % plateau.properties.size();
      Property* currentProperty = plateau.getProperty(position);
      cout << name << " est sur la case " << currentProperty->name << "." << endl;
      
      displayOwnedProperties();
      attemptPurchase(currentProperty);
      }
};

class Game {
    private:
      Plateau plateau;
      // Liste des joueurs
      vector<Player> players;

    public:
      // Démarre une partie avec des paramètres définis par l'utilisateur
      void startGame() {

        // 1. Définir le nombre de tours et de joueurs
        cout << endl << "------------------------------- Bienvenue dans le jeu de Monopoly simplifié ! -------------------------------" << endl << endl;

        // Demander à l'utilisateur combien de tours le jeu doit durer
        int iteration = getValidatedPositiveIntegerInput("Combien d'itérations (nombre de tour(s)) voulez-vous ?");

        // Demander à l'utilisateur combien de joueurs vont participer
        int numPlayers = getValidatedPositiveIntegerInput("Combien y a-t-il de joueurs ?");

        // Ajoute les joueurs au vecteur `players` avec leurs noms
        for (int i = 0; i < numPlayers; ++i) {
          string name;
          cout << "Nom du joueur : ";
          cin >> name;
          players.push_back(Player(name));
        }

        // Boucle principale : itère sur les tours de jeu
        for (int i = 0; i < iteration; i++) {
          cout << endl << endl << "-------------------------------" << " Tour numéro " << i + 1 << " -------------------------------" << endl;
          for (auto& player : players) {
            cout << endl << "---------- " << player.getName() << ", c'est votre tour ----------" << endl << endl;
            player.play();
          }
        }
      }

      /** Demande à l'utilisateur d'entrer un entier strictement positif et valide.
       * @param prompt Le message à afficher.
       * @return Un entier strictement positif.
       */
      int getValidatedPositiveIntegerInput(string prompt) {
        int value;
        do {
          cout << prompt << endl << ">>> ";
          cin >> value;
          if (cin.fail() || value <= 0) { // Si l'entrée n'est pas un entier
            cin.clear(); // Réinitialise l'état du flux
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Vide le buffer
            cout << "Erreur : veuillez entrer un entier strictement positif valide !" << endl;
          }
        } while (cin.fail() || value <= 0);
        return value;
      }
};

int main() {
  Game game;
  game.startGame();
  return 0;
}
