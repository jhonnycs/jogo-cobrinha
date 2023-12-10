#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // sleep
#include <time.h> // to include time function
#include <conio.h> // to include getch
#include <windows.h> // forçar tamanho da janela
#include <locale.h> // necessário para usar setlocale

char jogador[20]; //nome do jogador

#define LINHAS 26
#define COLUNAS 80
#define TAMANHO_COBRA 10
#define VELOCIDADE_FACIl 300 // menor, mais rápido
#define VELOCIDADE_MEDIO 200
#define VELOCIDADE_DIFICIL 100

// Key code
enum {
    KEY_ESC     = 27,
    KEY_ENTER   = 13,
    ARROW_UP    = 256 + 72,
    ARROW_DOWN  = 256 + 80,
    ARROW_LEFT  = 256 + 75,
    ARROW_RIGHT = 256 + 77
};

char menus[5][1000] = {
	"Jogar cobrinha",
	"Ver pontuacoes",
	"Como jogar?",
	"Sair do jogo"
};
char niveis[3][10] = {
	"facil",
	"medio",
	"dificil"
};

static int get_code();
void comoJogar();
void jogarCobrinha(int nivel);
void verPontuacoes();
void generateMenu(int choice);
int getRanking(int pontuacao);

int gerarComida();
int escolhaNivel();

void generateMenu(int choice) {
	system("cls");
	int count;
	printf("\n\n\n");
	for (count = 0; count < 4; count++){
		if(choice==count){
        	printf ("\t[*] %s\n",menus[count]);
		}else{
        	printf ("\t[ ] %s\n",menus[count]);
		}
	}

    SMALL_RECT windowSize = {0 , 0, COLUNAS, LINHAS};
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &windowSize);

    int ch;
    while (( ch = get_code()) != KEY_ESC ) {
        switch (ch) {
        case ARROW_UP:
            system("cls");
            if(choice > 0 ) choice--;
            generateMenu(choice);
            break;
        case ARROW_DOWN:
            system("cls");
            if(choice < 3 ) choice++;
            generateMenu(choice);
            break;
        case KEY_ENTER:
        	switch(choice){
        		case 0:
					int nivel = escolhaNivel();
        			jogarCobrinha(nivel);
        			break;
        		case 1:
        			verPontuacoes();
        			break;
        		case 2:
        			comoJogar();
        			break;
        		case 3:
        			printf("saindo....");
        			exit(0);
        			break;
        	}
			break;
        }
    }	   
}

void irColunaLinha(int coluna, int linha) {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),(COORD) {
        coluna,linha
    });
}

void carregarJogo() {
	system("cls");
	printf("Nome do jogador?\n");
	scanf("%[^\n]s", &jogador);
	system("cls");
	printf("Aguarde o jogo ser carregado...\n");
	char load[11] = "carregando";
	for (int i = 0; i < 11; ++i) {
		printf("%c ",load[i]);
		Sleep(80);
	}
	printf("...\nCarregamento concluido\n");
	sleep(1);
	printf("Iniciando...");
	system("cls");
}

void mostrarLimitesMatriz() {

	system("cls");
	int matriz[LINHAS][COLUNAS];

	for (int i = 0; i < LINHAS; ++i) {

		for (int j = 0; j < COLUNAS; ++j) {

			if((j==0) || (i==0) || (i== LINHAS-1 ) || (j== COLUNAS - 1)){
				printf("+");
			} else {
				printf(" ");
			}
		}
		printf("\n");
	}
}

void mostrarGameOver(int pontos) {
	// Salvar o recorde do joguinho
	// Abrindo o arquivo
	FILE *arquivo = fopen("ranking.txt", "a"); // append

	if(arquivo!=NULL){
		fprintf(arquivo, "%03d - %s\n", pontos, jogador);
		fclose(arquivo);
	} else {
		printf("Nenhum recorde registrado!\n");
	}

	Sleep(600);
	mostrarLimitesMatriz();
	irColunaLinha(4,4);
	printf("==============================");
	irColunaLinha(10,5);
	printf("G A M E  O V E R");
	irColunaLinha(10,6);
	printf("Pontuacao: %d", pontos);
	irColunaLinha(10,7);
	int posicao = getRanking(pontos);
	printf("posicao no ranking: %d%s", posicao, posicao == 0 ? "erro no arquivo" : "");
	irColunaLinha(4,8);
	printf("==============================");

	getch();
	system("cls");
	generateMenu(0);
}

// verde	+1 tamanho e pontos
// amarelo	+3 tamanho e pontos
// vermelho	+2 pontos
int gerarComida() {
	int pontoAleatorio = rand() % 100;
	if (pontoAleatorio < 49) {
		return 1;
	} else if (pontoAleatorio < 79) {
		return 3;
	} else {
		return 2;
	}
}

void gerarMenuNiveis(int choice) {
	system("cls");
	printf("Escolha a dificuldade:\n\n");
	for (int count = 0; count < 3; count++){
		if(choice==count) {
        	printf ("\t[*] %s\n",niveis[count]);
		} else {
        	printf ("\t[ ] %s\n",niveis[count]);
		}
	}
}

int escolhaNivel() {
	int nivel = 0, choice = 0;
	gerarMenuNiveis(0);

	int ch;
    while (( ch = get_code()) != KEY_ESC ) {
        switch (ch) {
			case(ARROW_UP):
				if (choice > 0) choice--;
				gerarMenuNiveis(choice);
				break;
			case(ARROW_DOWN):
				if (choice < 2) choice++;
				gerarMenuNiveis(choice);
				break;
			case(KEY_ENTER):
				nivel = choice + 1;
				break;
		}
		if (nivel != 0) break;
	}
	return nivel;
}

void jogarCobrinha(int nivel) {
	int cobraPosicaoX[COLUNAS] = {}; // Posição (linha) de inicio
	int cobraPosicaoY[LINHAS] = {}; // Posição (coluna) de inicio
	int comidaPosicaoX, comidaPosicaoY; // posições relativas aos pontos que irão surgir

	int i, tamanhoCobra = TAMANHO_COBRA; // tamanho da cobra
	int pontos = 0; // pontuação do jogador
	int ch; // código do charset (utilizado para definir a tecla pressionada)
	
	int velocidade;
	switch (nivel) {
	case 1:
		velocidade = VELOCIDADE_FACIl;
		break;
	case 2:
		velocidade = VELOCIDADE_MEDIO;
		break;
	case 3:
		velocidade = VELOCIDADE_DIFICIL;
		break;
	}
	
	carregarJogo();
	mostrarLimitesMatriz();

	// Gerar a posição do prmeiro ponto
	int comida = gerarComida();
    comidaPosicaoX=(rand() % (COLUNAS-2) )+1;
	comidaPosicaoY=(rand() % (LINHAS-2) )+1;


	char corComida;
	switch (comida) {
	case 1:
		corComida = '1';
		printf("\033[0;32m"); // Cor setada para verde
		break;
	case 2:
		corComida = '2';
		printf("\033[0;31m"); // Cor setada para vermelho
		break;
	case 3:
		corComida = '3';
		printf("\033[0;33m"); // Cor setada para amarelo
		break;
	}

	irColunaLinha(comidaPosicaoX, comidaPosicaoY);
	printf("%c", corComida);
	printf("\033[0m"); // cor setada para padrão


    // Mostrar informações
    irColunaLinha(0,LINHAS);
    printf("Precisone alguma tecla para iniciar...");

    // Gerar a posição da cobra de forma aleatorica
    cobraPosicaoX[0] = (rand()% (COLUNAS-2) )+1;
    cobraPosicaoY[0] = (rand()% (LINHAS-2) )+1;

    irColunaLinha(cobraPosicaoX[0], cobraPosicaoY[0]);
    printf("%c",'*'); 

    ch = get_code(); // Aguardar uma tecla para iniciar

 	while (ch != KEY_ESC ) {
 		
 		// Limpar instruções
 		for (int i = 0; i < 37; ++i) {

 			irColunaLinha(i, LINHAS);
 			printf(" ");
 		}

        // Verificar se a cobrinha bateu na parede
        if(
        	(cobraPosicaoY[0] == 0 )  		||
        	(cobraPosicaoY[0] == LINHAS ) 	||
        	(cobraPosicaoX[0] == 0 )  		||
        	(cobraPosicaoX[0] == (COLUNAS-1) )
		)
        {
        	mostrarGameOver(pontos);
        }

 		// Acompanhar os movimentos da compra
		for(i = tamanhoCobra; i > 0; i--) {
        	cobraPosicaoX[i]=cobraPosicaoX[i-1];
            cobraPosicaoY[i]=cobraPosicaoY[i-1];
        }

       	// Apagar rastro
        irColunaLinha(cobraPosicaoX[tamanhoCobra],cobraPosicaoY[tamanhoCobra]);
        printf(" ");

		// pegar a tecla para mudar a direção da cobra
        if(kbhit()) {
        	ch = get_code();
        }

        // Verificamos o código da tecla que foi pressionada para fazer a ação correta
        switch (ch) {
	        case ARROW_UP:
	        	cobraPosicaoY[0]--;
	            break;
	        case ARROW_DOWN:
	        	cobraPosicaoY[0]++;
	            break;
	        case ARROW_LEFT:
	        	cobraPosicaoX[0]--;
	            break;
	        case ARROW_RIGHT:
	        	cobraPosicaoX[0]++;
	            break;
            default:
            	ch = get_code();
            	break;
        }

        // Verificar se a cobrinha comeu alguma comida
        if(	comidaPosicaoX == cobraPosicaoX[0] &&
			comidaPosicaoY == cobraPosicaoY[0] )
			{
			
			PlaySound("som.wav", NULL, SND_FILENAME || SND_ASYNC);
			if (comida == 1 || comida == 3) {
				tamanhoCobra = tamanhoCobra + comida;
			}
            pontos = pontos + comida;
            
			// Gerar a próxima comida
            comidaPosicaoX=(rand() % (COLUNAS-2) )+1;
            comidaPosicaoY=(rand() % (LINHAS-2) )+1;
			comida = gerarComida();

			
			// Indo para a posição para imprimir o proximo ponto
			irColunaLinha(comidaPosicaoX, comidaPosicaoY);
			char corComida;
			switch (comida) {
			case 1:
				corComida = '1';
				printf("\033[0;32m"); // Cor setada para verde
				break;
			case 2:
				corComida = '2';
				printf("\033[0;31m"); // Cor setada para vermelho
				break;
			case 3:
				corComida = '3';
				printf("\033[0;33m"); // Cor setada para amarelo
				break;
			}

			printf("%c", corComida);
			printf("\033[0m"); // cor setada para padrão
        }

        // Indo para a posição para imprimir a cobrinha
        irColunaLinha(cobraPosicaoX[0],cobraPosicaoY[0]);
		printf("%c", '*');

        // Atualizar o placar
	    irColunaLinha(0,LINHAS);
	    printf("Pontos: %d - %s", pontos, jogador);

		Sleep(velocidade);
        
		// Verificar se a cobra bateu nela mesma
        for(i=1; i < tamanhoCobra; i++) {
            if(cobraPosicaoX[0]==cobraPosicaoX[i]&&cobraPosicaoY[0]==cobraPosicaoY[i]){
				mostrarGameOver(pontos);
            }
        }
    }

	getch();
	system("cls");
	generateMenu(0);
}

void verPontuacoes(){
	system("cls");
	char texto_str[20];
	FILE *arquivo = fopen("ranking.txt", "r");

	if (arquivo != NULL) {
		printf("Listando:\n");
		while(fgets(texto_str, 20, arquivo) != NULL){
			printf("%s", texto_str);
		}
		
		fclose(arquivo);

	} else {
		printf("Nenhuma pontuacao registrada!\n");
	}

	getch();
	system("cls");
	generateMenu(1);	
}

void comoJogar() {
	system("cls");
	printf("Utilize as setinhas para jogar\n");
	getch();
	system("cls");
	generateMenu(2);
}

static int get_code() {
    int ch = getch();
    if (ch == 0 || ch == 224)
        ch = 256 + getch();
    return ch;
}

int getRanking(int pontuacao) {
	FILE *f = fopen("ranking.txt", "r");
	char texto[20];
	int quantP = 0;
	int pontuacoesMaiores = 0;

	if (f != NULL) {
		while (fgets(texto, 20, f) != NULL) {
			quantP++;
		}

		fseek(f, 0, SEEK_SET);
		int pontAtual;

		for (int i = 0; i < quantP; i++) {
			fscanf(f, "%d", &pontAtual);

			if (pontAtual > pontuacao) {
				pontuacoesMaiores++;
			}
			fgets(texto, 20, f);
		}
		int posicao = pontuacoesMaiores + 1;
		fclose(f);
		return posicao;
	} else {
		return 0;
	}
}

int main(int argc, char const *argv[]) {
	time_t t;
	srand((unsigned) time(&t));	
	setlocale(LC_ALL,"");
	generateMenu(0);
	return 0;
}
