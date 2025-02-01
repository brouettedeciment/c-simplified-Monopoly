#include <random>
#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
#include <math.h>
using namespace std;

class Player; // Déclaration anticipée

class Utils {
  public:
    /** Demande à l'utilisateur une entrée valide (Oui/Non) et retourne un bool.
     * @param prompt Le message à afficher.
     * @param valid_yes Liste des réponses considérées comme "Oui".
     * @param valid_no Liste des réponses considérées comme "Non".
     * @return true si la réponse est dans valid_yes, false si dans valid_no.
     */
    bool getValidatedPositiveNegativeStringInput(string prompt, vector<string> valid_yes = {"O", "o"}, vector<string> valid_no = {"N", "n"}) {
      string local_input;
      bool valid = false;

      auto printValidAnswers = [&valid_yes, &valid_no]() {
        cout << " (";
        for (size_t i = 0; i < valid_yes.size(); i++) {
          cout << valid_yes[i];
          if (i != valid_yes.size() - 1) cout << "/";
        }
        cout << " - ";
        for (size_t i = 0; i < valid_no.size(); i++) {
          cout << valid_no[i];
          if (i != valid_no.size() - 1) cout << "/";
        }
        cout << ")";
      };

      do {
        cout << prompt;
        printValidAnswers();
        cout << endl << ">>> ";

        cin >> local_input;

        if (find(valid_yes.begin(), valid_yes.end(), local_input) != valid_yes.end()) {
          return true;
        } else if (find(valid_no.begin(), valid_no.end(), local_input) != valid_no.end()) {
          return false;
        } else {
          cout << "Erreur : entrée invalide, veuillez répondre par";
          printValidAnswers();
          cout << " !" << endl;
        }
      } while (!valid);

      return false; // Ne devrait jamais être atteint
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
        {"depart", 0}, {"boulevard De Belleville", 60}, {"caisse Communaute", 0}, {"rue Lecourbe", 60}, {"impots", -200}, 
        {"gare Montparnasse", 200}, {"rue De Vaugirard", 100}, {"chance", 0}, {"rue De Courcelles", 100}, {"avenue De La Republique", 120},

        {"prison", 0}, {"boulevard De La Villette", 140}, {"electricite", 150}, {"avenue De Neuilly", 140}, {"rue De Paradis", 160}, 
        {"gare De Lyon", 200}, {"avenue Mozart", 180}, {"caisse Communaute", 0}, {"boulevard ST-Michel", 180}, {"place Pigalle", 200},

        {"parc", 0}, {"avenue Matignon", 220}, {"chance", 0}, {"boullevrad Malesherbes", 220}, {"avenue Henri-Martin", 240}, 
        {"gare Du Nord", 200}, {"foubourg ST-Honore", 260}, {"place De La Bourse", 260}, {"eaux", 150}, {"rue La Fayette", 280},

        {"allez Prison", 0}, {"avenue De Breteuil", 300}, {"avenue Foch", 300}, {"caisse Communaute", 0}, {"boulevard Des Capucines", 320}, 
        {"gare ST-Lazare", 200}, {"chance", 0}, {"avenue Des Champs-Elysee", 350}, {"taxe", -200}, {"rue De LaPaix", 400}
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
    Utils utils;
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
      } else if (currentProperty->price < 0) {
        cout << "Vous devez : " << abs(currentProperty->price) << " skibidibucks a la banque.";
        money += currentProperty->price;
      } else if (currentProperty->price > 0){

        if (utils.getValidatedPositiveNegativeStringInput("voulez-vous acheter cette propriete pour " + to_string(currentProperty->price) + " skibidibucks")) {
        currentProperty->owner = this; // Définir le propriétaire
        ownedProperties.push_back(currentProperty); // Ajouter au joueur
        money -= currentProperty->price;
        cout << name << " a acheté " << currentProperty->name << " pour " << currentProperty->price << " skibidibucks." << endl;
          }
      }
    }

    void upgradeProperty() {
      if (utils.getValidatedPositiveNegativeStringInput("voulez-vous ameliorer une de vos propriete")) {
        string local_input;
        cout << "Quelle propriete souhaitez-vous ameliorer : ";
        for (size_t i = 0; i < ownedProperties.size(); i++) {
          cout << i << ". " << ownedProperties[i]->name;
          if (ownedProperties[i]->hotel) {
            cout << ", 1 hotel";
          } else {
            cout << ", " << ownedProperties[i]->houses << " maison(s)";
          }
          if (i != ownedProperties.size() - 1) {
            cout << " | "; // Ajouter une virgule entre les propriétés sauf pour la dernière
          }
        }
        cout << "." << endl << ">>> ";
        cin >> local_input;
        if (ownedProperties[stoi(local_input)]->houses == 4) {
          ownedProperties[stoi(local_input)]->houses = 0;
          ownedProperties[stoi(local_input)]->hotel = true;
          money -= ownedProperties[stoi(local_input)]->getRent();
        } else {
          ownedProperties[stoi(local_input)]->houses += 1;
          money -= ownedProperties[stoi(local_input)]->getRent();
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
      if (!ownedProperties.empty()){
        upgradeProperty();
      }
    }
};

class Game {
    private:
      Plateau plateau;
      Utils utils;
      // Liste des joueurs
      vector<Player> players;

    public:
      // Démarre une partie avec des paramètres définis par l'utilisateur
      void startGame() {

        // 1. Définir le nombre de tours et de joueurs
        cout << endl << "------------------------------- Bienvenue dans le jeu de Monopoly simplifié ! -------------------------------" << endl << endl;

        // Demander à l'utilisateur combien de tours le jeu doit durer
        int iteration = utils.getValidatedPositiveIntegerInput("Combien d'itérations (nombre de tour(s)) voulez-vous ?");

        // Demander à l'utilisateur combien de joueurs vont participer
        int numPlayers = utils.getValidatedPositiveIntegerInput("Combien y a-t-il de joueurs ?");

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
};

int main() {
  Game game;
  game.startGame();
  return 0;
}
