#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_INVENTORY 3
#define MAX_COMMAND 50
#define NUM_ROOMS 9

typedef struct Room
{
    char *name;
    char *description;
    struct Room *connections[4];
    char *monster;
    int monsterHealth;
    char *loot;
} Room;

typedef struct
{
    int health;
    char *inventory[MAX_INVENTORY];
    int inventoryCount;
    Room *currentRoom;
} Player;

typedef struct
{
    Player player; 
    Room *rooms[NUM_ROOMS];
} Game;

Room *createRoom(const char *name, const char *description, const char *monster, int monsterHealth, const char *loot)
{
    Room *room = malloc(sizeof(Room));
    room->name = strdup(name);
    room->description = strdup(description);
    room->monster = monster ? strdup(monster) : NULL;
    room->monsterHealth = monsterHealth;
    room->loot = loot ? strdup(loot) : NULL;

    for (int i = 0; i < 4; i++)
    {
        room->connections[i] = NULL;
    }
    return room;
}

void initializeGame(Game *game)
{
    for (int i = 0; i < NUM_ROOMS; i++)
    {
        game->rooms[i] = NULL;
    }

    game->rooms[0] = createRoom("Room 1", "The starting point of your journey.", "Goblin", 30, "Knife");
    game->rooms[1] = createRoom("Room 2", "A mysterious ancient hall.", "Orc", 50, "Shield");
    game->rooms[2] = createRoom("Room 3", "A quiet library filled with secrets.", "Spider", 40, "Bow");
    game->rooms[3] = createRoom("Room 4", "A dimly lit stone pathway.", "Bat", 20, "Arrow");
    game->rooms[4] = createRoom("Room 5", "The main cavern with a golden glow.", "Wolf", 45, "Ancient Spell Book");
    game->rooms[5] = createRoom("Room 6", "An ancient temple with mystical energies.", "Dragon", 60, "Dragon Glass Axe");
    game->rooms[6] = createRoom("Room 7", "A treasure trove room.", "Treasure Goblin", 20, "Gold Dagger");
    game->rooms[7] = createRoom("Room 8", "A dark tunnel leading to secrets.", "White Walker", 25, "Guard Dog");
    game->rooms[8] = createRoom("Room 9", "The final resting place, quiet but eerie.", "Necromancer", 80, "A Healing Plant");

    game->rooms[0]->connections[1] = game->rooms[1];
    game->rooms[1]->connections[3] = game->rooms[0];
    game->rooms[1]->connections[1] = game->rooms[2];
    game->rooms[2]->connections[3] = game->rooms[1];
    game->rooms[2]->connections[2] = game->rooms[3];
    game->rooms[3]->connections[1] = game->rooms[4];
    game->rooms[4]->connections[1] = game->rooms[5];
    game->rooms[5]->connections[2] = game->rooms[6];
    game->rooms[6]->connections[1] = game->rooms[7];
    game->rooms[7]->connections[2] = game->rooms[8];

    game->player.health = 100;
    game->player.inventoryCount = 0;
    game->player.currentRoom = game->rooms[0];
}
//Explanation for the room that player is in
void lookAround(Player *player)
{
    printf("You are in %s.\n%s\n", player->currentRoom->name, player->currentRoom->description);

    if (player->currentRoom->monster)
    {
        printf("You see a monster here: %s (Health: %d)\n", player->currentRoom->monster, player->currentRoom->monsterHealth);

        if (player->currentRoom->loot)
        {
            printf("The monster has loot: %s\n", player->currentRoom->loot);
        }
    }
    else
    {
        printf("There are no monsters here.\n");
    }
}

void movePlayer(Player *player, int direction)
{
    if (player->currentRoom->connections[direction])
    {
        player->currentRoom = player->currentRoom->connections[direction];
        printf("You moved to %s.\n", player->currentRoom->name);
    }
    else
    {
        printf("You cannot move in that direction.\n");
    }
}
int attackMonster(Player *player)
{
    if (player->currentRoom->monster)
    {
        printf("You attack %s!\n", player->currentRoom->monster);
        player->currentRoom->monsterHealth -= 20;

        if (player->currentRoom->monsterHealth <= 0)
        {
            printf("You defeated the %s!\n", player->currentRoom->monster);

            free(player->currentRoom->monster);
            player->currentRoom->monster = NULL;
            return 1;
        }
        else
        {
            printf("The monster has %d health remaining.\n", player->currentRoom->monsterHealth);
            return 0;
        }
    }
    else
    {
        printf("There are no monsters here to attack.\n");
        return 0;
    }
}

void pickupLoot(Player *player, const char *item)
{
    if (!player->currentRoom->loot)
    {
        printf("There is no loot to pick up here.\n");
        return;
    }

    if (strcmp(player->currentRoom->loot, item) != 0)
    {
        printf("The specified loot %s is not available in this room.\n", item);
        return;
    }

    for (int i = 0; i < player->inventoryCount; i++)
    {
        if (strcmp(player->inventory[i], item) == 0)
        {
            printf("You already have %s in your inventory.\n", item);
            return;
        }
    }

    if (player->inventoryCount < MAX_INVENTORY)
    {
        player->inventory[player->inventoryCount++] = strdup(player->currentRoom->loot);
        printf("You picked up %s!\n", player->currentRoom->loot);
        free(player->currentRoom->loot);
        player->currentRoom->loot = NULL;
    }
    else
    {
        printf("Your inventory is full. Cannot pick up %s.\n", player->currentRoom->loot);
    }
}

void saveGame(Game *game, const char *filepath)
{
    FILE *file = fopen(filepath, "w");
    if (!file)
    {
        printf("Failed to save the game.\n");
        return;
    }

    fprintf(file, "=== Game Save File ===\n");
    fprintf(file, "Player Health: %d\n", game->player.health);
    fprintf(file, "Inventory Count: %d\n", game->player.inventoryCount);

    fprintf(file, "Inventory:\n");
    for (int i = 0; i < game->player.inventoryCount; i++)
    {
        fprintf(file, "%s\n", game->player.inventory[i]);
    }

    int roomIndex = game->player.currentRoom - game->rooms[0];
    fprintf(file, "Current Room: %d\n", roomIndex);

    fclose(file);
    printf("Game saved successfully to %s.\n", filepath);
}

void loadGame(Game *game, const char *filepath)
{
    FILE *file = fopen(filepath, "r");
    if (!file)
    {
        printf("Failed to load the game.\n");
        return;
    }

    char buffer[100];
    int inventoryCount = 0;

    fgets(buffer, sizeof(buffer), file); // "=== Game Save File ==="

    fscanf(file, "Player Health: %d\n", &game->player.health);
    fscanf(file, "Inventory Count: %d\n", &inventoryCount);

    fgets(buffer, sizeof(buffer), file); // "Inventory:"
    for (int i = 0; i < inventoryCount; i++)
    {
        fgets(buffer, sizeof(buffer), file);
        buffer[strcspn(buffer, "\n")] = 0;
        game->player.inventory[i] = strdup(buffer);
    }
    game->player.inventoryCount = inventoryCount;

    int roomIndex = 0;
    fscanf(file, "Current Room: %d\n", &roomIndex);
    game->player.currentRoom = game->rooms[roomIndex];

    fclose(file);
    printf("Game loaded successfully from %s.\n", filepath);
}

void displayInventory(Player *player)
{
    printf("Your inventory:\n");
    if (player->inventoryCount == 0)
    {
        printf("  (empty)\n");
    }
    else
    {
        for (int i = 0; i < player->inventoryCount; i++)
        {
            printf("  %d: %s\n", i + 1, player->inventory[i]);
        }
    }
}

void handleCommand(Game *game, char *command)
{
    if (strcmp(command, "save") == 0)
    {
        saveGame(game, "savegame.txt");
    }
    else if (strcmp(command, "load") == 0)
    {
        loadGame(game, "savegame.txt");
    }
    else if (strcmp(command, "look") == 0)
    {
        lookAround(&game->player);
    }
    else if (strcmp(command, "inventory") == 0)
    {
        displayInventory(&game->player);
    }
    else if (strncmp(command, "drop ", 5) == 0)
    {
        char *item_to_drop = command + 5;
        for (int i = 0; i < game->player.inventoryCount; i++)
        {
            if (strcmp(game->player.inventory[i], item_to_drop) == 0)
            {
                free(game->player.inventory[i]);
                for (int j = i; j < game->player.inventoryCount - 1; j++)
                {
                    game->player.inventory[j] = game->player.inventory[j + 1];
                }
                game->player.inventory[game->player.inventoryCount - 1] = NULL;
                game->player.inventoryCount--;

                printf("You dropped %s.\n", item_to_drop);
                return;
            }
        }
        printf("You don't have %s in your inventory.\n", item_to_drop);
    }

    else if (strncmp(command, "pickup ", 7) == 0)
    {
        char *item_to_pickup = command + 7;
        pickupLoot(&game->player, item_to_pickup);
    }

    else if (strcmp(command, "attack") == 0)
    {
        int monster_defeated = attackMonster(&game->player);

        if (monster_defeated && game->player.currentRoom->loot)
        {
            printf("You can now pick up loot using the `pickup <item>` command.\n");
        }
    }
    else if (strncmp(command, "move ", 5) == 0)
    {
        char direction[20];
        sscanf(command + 5, "%19s", direction);

        if (strcmp(direction, "north") == 0)
        {
            movePlayer(&game->player, 0);
        }
        else if (strcmp(direction, "south") == 0)
        {
            movePlayer(&game->player, 1);
        }
        else if (strcmp(direction, "east") == 0)
        {
            movePlayer(&game->player, 2);
        }
        else if (strcmp(direction, "west") == 0)
        {
            movePlayer(&game->player, 3);
        }
        else
        {
            printf("Unknown direction. Use north, south, east, or west.\n");
        }
    }
    else
    {
        printf("Unknown command.\n");
    }
}

int main()
{
    Game game;
    char command[MAX_COMMAND];

    
    initializeGame(&game);

    printf("Welcome to the adventure game!\n");
    printf("Commands: move north/south/east/west, attack, look, inventory, drop <item>, save, load, exit\n");

    while (1)
    {
        printf("\nEnter command: ");
        fgets(command, MAX_COMMAND, stdin);
        command[strcspn(command, "\n")] = 0;

        if (strcmp(command, "exit") == 0)
        {
            printf("Goodbye!\n");
            break;
        }

        handleCommand(&game, command);
    }

    for (int i = 0; i < NUM_ROOMS; i++)
    {
        free(game.rooms[i]->name);
        free(game.rooms[i]->description);
        free(game.rooms[i]->monster);
        free(game.rooms[i]->loot);
        free(game.rooms[i]);
    }

    printf("Game resources cleaned up. Goodbye!\n");

    return 0;
}
