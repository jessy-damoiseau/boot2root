#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LEN 80
#define MAX_STRINGS 6

int num_input_strings = 0;
char input_strings[MAX_STRINGS][MAX_INPUT_LEN];

// Table pour phase 5
char array_123[16] = { 'i','s','r','v','e','a','w','h','o','b','p','n','u','t','f','g' };

// Arbre pour phase secrète (simulé ici)
typedef struct TreeNode {
    int value;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

TreeNode n1 = { 253, NULL, NULL };
TreeNode n2 = { 725, NULL, NULL };
TreeNode n3 = { 301, NULL, NULL };
TreeNode n4 = { 997, NULL, NULL };
TreeNode n5 = { 212, NULL, NULL };
TreeNode n6 = { 432, NULL, NULL };
TreeNode n7 = { 107, NULL, NULL };

// Construit manuellement
void init_tree() {
    n1.left = &n2; n1.right = &n3;
    n2.left = &n4; n2.right = &n5;
    n3.left = &n6; n3.right = &n7;
}

void explode_bomb() {
    printf("\nBOOM!!!\nThe bomb has blown up.\n");
    exit(1);
}

int strings_not_equal(const char *a, const char *b) {
    return strcmp(a, b) != 0;
}

int string_length(const char *s) {
    return strlen(s);
}

int read_six_numbers(const char *s, int *arr) {
    return sscanf(s, "%d %d %d %d %d %d", &arr[0], &arr[1], &arr[2], &arr[3], &arr[4], &arr[5]) == 6;
}

void phase_1(const char *input) {
    if (strings_not_equal(input, "Public speaking is very easy.")) explode_bomb();
}

void phase_2(const char *input) {
    int nums[6];
    if (!read_six_numbers(input, nums)) explode_bomb();
    if (nums[0] != 1) explode_bomb();
    for (int i = 1; i < 6; ++i) {
        if (nums[i] != nums[i-1] * (i+1)) explode_bomb();
    }
}

void phase_3(const char *input) {
    int index, val;
    char letter;
    if (sscanf(input, "%d %c %d", &index, &letter, &val) < 3) explode_bomb();

    switch (index) {
        case 0: if (letter != 'q' || val != 777) explode_bomb(); break;
        case 1: if (letter != 'b' || val != 214) explode_bomb(); break;
        case 2: if (letter != 'b' || val != 755) explode_bomb(); break;
        case 3: if (letter != 'k' || val != 251) explode_bomb(); break;
        case 4: if (letter != 'o' || val != 160) explode_bomb(); break;
        case 5: if (letter != 't' || val != 458) explode_bomb(); break;
        case 6: if (letter != 'v' || val != 780) explode_bomb(); break;
        case 7: if (letter != 'b' || val != 524) explode_bomb(); break;
        default: explode_bomb();
    }
}

int func4(int n) {
    if (n <= 1) return 1;
    return func4(n-1) + func4(n-2);
}

void phase_4(const char *input) {
    int val;
    if (sscanf(input, "%d", &val) != 1 || val <= 0) explode_bomb();
    if (func4(val) != 55) explode_bomb();
}

void phase_5(const char *input) {
    if (string_length(input) != 6) explode_bomb();
    char result[7] = {0};
    for (int i = 0; i < 6; ++i) {
        result[i] = array_123[input[i] & 0xF];
    }
    if (strings_not_equal(result, "giants")) explode_bomb();
}

typedef struct ListNode {
    int value;
    struct ListNode* next;
} ListNode;

// Linked list values for phase 6
ListNode nodes[6] = {
    { 850, NULL }, { 720, NULL }, { 660, NULL },
    { 500, NULL }, { 410, NULL }, { 300, NULL },
};

ListNode* indices[6] = { &nodes[0], &nodes[1], &nodes[2], &nodes[3], &nodes[4], &nodes[5] };

void phase_6(const char *input) {
    int indices_input[6];
    if (!read_six_numbers(input, indices_input)) explode_bomb();

    // Check valid and unique
    for (int i = 0; i < 6; ++i) {
        if (indices_input[i] < 1 || indices_input[i] > 6) explode_bomb();
        for (int j = i + 1; j < 6; ++j) {
            if (indices_input[i] == indices_input[j]) explode_bomb();
        }
    }

    ListNode* head = indices[indices_input[0] - 1];
    ListNode* current = head;

    for (int i = 1; i < 6; ++i) {
        current->next = indices[indices_input[i] - 1];
        current = current->next;
    }
    current->next = NULL;

    // Check descending
    current = head;
    while (current->next) {
        if (current->value < current->next->value)
            explode_bomb();
        current = current->next;
    }
}

int fun7(TreeNode* node, int val) {
    if (!node) return -1;
    if (val < node->value) return 2 * fun7(node->left, val);
    if (val == node->value) return 0;
    return 2 * fun7(node->right, val) + 1;
}

void secret_phase() {
    char line[MAX_INPUT_LEN];
    if (!fgets(line, MAX_INPUT_LEN, stdin)) explode_bomb();

    int target = strtol(line, NULL, 10);
    if (target < 1 || target > 1000) explode_bomb();

    if (fun7(&n1, target) != 7) explode_bomb();

    printf("Wow! You've defused the secret stage!\n");
    printf("Congratulations! You've defused the bomb!\n");
}

void phase_defused(const char *last_line) {
    int x;
    char word[64];
    if (sscanf(last_line, "%d %s", &x, word) == 2 && strcmp(word, "austinpowers") == 0) {
        printf("Curses, you've found the secret phase!\n");
        printf("But finding it and solving it are quite different...\n");
        secret_phase();
        return;
    }

    printf("Congratulations! You've defused the bomb!\n");
}

int main() {
    init_tree();

    char line[MAX_INPUT_LEN];

    for (int i = 0; i < 6; ++i) {
        if (!fgets(line, MAX_INPUT_LEN, stdin)) explode_bomb();
        line[strcspn(line, "\n")] = '\0'; // Remove newline
        strcpy(input_strings[i], line);
    }

    phase_1(input_strings[0]);
    phase_2(input_strings[1]);
    phase_3(input_strings[2]);
    phase_4(input_strings[3]);
    phase_5(input_strings[4]);
    phase_6(input_strings[5]);

    // Try to read secret phase trigger
    if (fgets(line, MAX_INPUT_LEN, stdin)) {
        line[strcspn(line, "\n")] = '\0';
        phase_defused(line);
    } else {
        printf("Congratulations! You've defused the bomb!\n");
    }

    return 0;
}
