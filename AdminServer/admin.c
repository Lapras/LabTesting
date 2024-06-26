#include <stdio.h>

void pwnme() {
    char buf[100];

    puts ("So what do you have for input?");
    puts("I may have forogtten to turn off NoX >");
    read (0, &buf, 0x200);

    puts("Thanks!");
    return;
}

int main() {
    setvbuf(stdout, (char*) 0x0, 2, 0);
    puts("Welcome to the admin server! There is no password. have fun slamming your head against a brick wall.");
    puts("x86_64");
    pwnme();
    return 0;
}

