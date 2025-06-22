#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define NUM_AVENTUREIROS 5
#define VIDA_INICIAL 100

const char *nomes_classes[] = {"Guerreiro", "Mago", "Caçador", "Paladino", "Bárbaro"};

int ataques[] = {20, 30, 18, 15, 25};
int defesas[] = {10, 5, 8, 12, 6};

typedef struct {
    int classe;
    int vida;
    int ataque;
    int defesa;
    int habilidade_ativa;
} Personagem;

void inicializarEquipe(Personagem equipe[]) {
    for (int i = 0; i < NUM_AVENTUREIROS; i++) {
        equipe[i].classe = i;
        equipe[i].vida = VIDA_INICIAL;
        equipe[i].ataque = ataques[i];
        equipe[i].defesa = defesas[i];
        equipe[i].habilidade_ativa = 0;
    }
}

int vivos(Personagem equipe[]) {
    int count = 0;
    for (int i = 0; i < NUM_AVENTUREIROS; i++) {
        if (equipe[i].vida > 0) count++;
    }
    return count;
}

int escolherAtacante(Personagem equipe[]) {
    int melhor = -1;
    float melhorRazao = -1.0;
    for (int i = 0; i < NUM_AVENTUREIROS; i++) {
        if (equipe[i].vida > 0) {
            float razao = (float)equipe[i].vida / equipe[i].ataque;
            if (razao > melhorRazao) {
                melhorRazao = razao;
                melhor = i;
            }
        }
    }
    return melhor;
}

int escolherAlvo(Personagem equipe[]) {
    int vivosIndices[NUM_AVENTUREIROS];
    int total = 0;
    for (int i = 0; i < NUM_AVENTUREIROS; i++) {
        if (equipe[i].vida > 0) vivosIndices[total++] = i;
    }
    if (total == 0) return -1;
    return vivosIndices[rand() % total];
}

int chance(int porcentagem) {
    return rand() % 100 < porcentagem;
}

int calcularDano(Personagem *atacante, Personagem *defensor) {
    int dano = atacante->ataque;

    if (chance(20)) {
        printf("  → Ataque errou! Nenhum dano aplicado.\n");
        return 0;
    }

    if (!chance(20)) {
        dano -= defensor->defesa;
    }

    if (atacante->classe == 0 && chance(20)) {
        printf("  → Habilidade do Guerreiro ativada: Golpe Crítico!\n");
        dano *= 2;
    }
    if (atacante->classe == 1 && chance(25)) {
        printf("  → Habilidade do Mago ativada: Bola de Fogo!\n");
        dano = atacante->ataque;
    }
    if (atacante->classe == 2 && chance(15)) {
        printf("  → Habilidade do Caçador ativada: Ataque Duplo!\n");
        return calcularDano(atacante, defensor) + calcularDano(atacante, defensor);
    }
    if (atacante->classe == 4) {
        printf("  → Habilidade do Bárbaro ativada: Nunca erra!\n");
        dano = atacante->ataque - defensor->defesa;
    }

    if (dano < 0) dano = 0;
    return dano;
}

void aplicarDano(Personagem *defensor, int dano) {
    defensor->vida -= dano;
    if (defensor->vida < 0) defensor->vida = 0;
}

void habilidadePaladino(Personagem *paladino, int vida_antes) {
    if (paladino->classe == 3 && paladino->vida > 0 && chance(30)) {
        int vida_perdida = vida_antes - paladino->vida;
        int recuperada = vida_perdida * 0.2;
        paladino->vida += recuperada;
        if (paladino->vida > VIDA_INICIAL) paladino->vida = VIDA_INICIAL;
        printf("  → Habilidade do Paladino ativada: Regenera %d de vida!\n", recuperada);
    }
}

void imprimirEstado(Personagem equipe1[], Personagem equipe2[]) {
    printf(">>> Estado Atual:\n");
    printf("Time 1:\n");
    for (int i = 0; i < NUM_AVENTUREIROS; i++) {
        printf(" %s: Vida %d\n", nomes_classes[equipe1[i].classe], equipe1[i].vida);
    }
    printf("Time 2:\n");
    for (int i = 0; i < NUM_AVENTUREIROS; i++) {
        printf(" %s: Vida %d\n", nomes_classes[equipe2[i].classe], equipe2[i].vida);
    }
    printf("\n");
}

int main() {
    srand(time(NULL));

    Personagem equipe1[NUM_AVENTUREIROS];
    Personagem equipe2[NUM_AVENTUREIROS];
    inicializarEquipe(equipe1);
    inicializarEquipe(equipe2);

    int turno = rand() % 2;
    int rodada = 1;
    printf("Time %d começa!\n\n", turno + 1);

    while (vivos(equipe1) > 0 && vivos(equipe2) > 0) {
        printf(">>> Rodada %d:\n", rodada);

        Personagem *atacante, *defensor;
        Personagem *time_atual = turno == 0 ? equipe1 : equipe2;
        Personagem *time_oponente = turno == 0 ? equipe2 : equipe1;

        int i_atacante = escolherAtacante(time_atual);
        int i_defensor = escolherAlvo(time_oponente);

        atacante = &time_atual[i_atacante];
        defensor = &time_oponente[i_defensor];

        printf("Time %d — %s (Vida: %d) ataca %s do Time %d (Vida: %d)\n",
            turno + 1, nomes_classes[atacante->classe], atacante->vida,
            nomes_classes[defensor->classe], (turno == 0 ? 2 : 1), defensor->vida);

        int vida_antes = defensor->vida;
        int dano = calcularDano(atacante, defensor);
        printf("  → Dano aplicado: %d\n", dano);
        aplicarDano(defensor, dano);
        if (defensor->classe == 3) habilidadePaladino(defensor, vida_antes);

        turno = 1 - turno;
        rodada++;
        imprimirEstado(equipe1, equipe2);
    }

    if (vivos(equipe1) > vivos(equipe2)) printf("Time 1 venceu!\n");
    else if (vivos(equipe2) > vivos(equipe1)) printf("Time 2 venceu!\n");
    else printf("Empate!\n");

    return 0;
}
