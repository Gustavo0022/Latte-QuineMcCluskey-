#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <set>
#include <map>
#include <fstream>   
#include <sstream>   

using namespace std;

ofstream output;

//converte as entradas para binário, adicionando 0 na frente caso necessário
string converterParaBinario(int numero, int numVariaveis) {
    if (numero == 0) {
        return string(numVariaveis, '0');
    }
    string binario;
    while (numero > 0) {
        binario += to_string(numero % 2);
        numero /= 2;
    }
    while (binario.length() < numVariaveis) {
        binario += '0';
    }
    reverse(binario.begin(), binario.end());
    return binario;
}

struct Termo {
    string binario;
    vector<int> mintermos;
    int contagemDeUns;
    bool foiCombinado = false;
    Termo() = default;
    Termo(int posicao, uint64_t numVariaveis) {
        this->binario = converterParaBinario(posicao, numVariaveis);
        this->mintermos.push_back(posicao);
        this->contagemDeUns = 0;
        for (char c : this->binario) {
            if (c == '1') {
                this->contagemDeUns++;
            }
        }
    }
};


//combina termos com diferença de apenas 1 bit (complementares)
Termo combinarTermos(const Termo& t1, const Termo& t2) {
    int diferencas = 0; //contador de diferenças
    string novoBinario = t1.binario;
    
    //verificação dos bits dos termos, e substituição por traço se forem complementares
    for (size_t i = 0; i < t1.binario.length(); ++i) {
        if (t1.binario[i] != t2.binario[i]) {
            diferencas++;
            novoBinario[i] = '-';
        }
    }
    if (diferencas == 1) { //se o número de diferenças for igual a 1
        //instância do novo termo, copiando entradas e eliminando correspondências duplicadas
        Termo novoTermo; 
        novoTermo.binario = novoBinario;
        novoTermo.mintermos = t1.mintermos;
        novoTermo.mintermos.insert(novoTermo.mintermos.end(), t2.mintermos.begin(), t2.mintermos.end());
        sort(novoTermo.mintermos.begin(), novoTermo.mintermos.end());
        novoTermo.mintermos.erase(unique(novoTermo.mintermos.begin(), novoTermo.mintermos.end()), novoTermo.mintermos.end());
        return novoTermo;
    }
    return Termo();
}

//impressão dos termos (não funciona muito bem se forem mais de 26 variáveis)
void imprimirTermo(const Termo& termo) {
    bool termoVazio = true;
    for (size_t i = 0; i < termo.binario.length(); ++i) {
        if (termo.binario[i] == '1') {
            cout << (char)('A' + i);
            termoVazio = false;
        } else if (termo.binario[i] == '0') {
            cout << (char)('A' + i) << "'";
            termoVazio = false;
        }
    }
    if (termoVazio) {
        cout << "1";
    }
}

//impressão da tabela verdade simplificada (melhor para visualização de circuitos de grande escala)
void imprimirTabela(vector<Termo> termos) {
    for(Termo t : termos){
        cout << t.binario <<" 1" <<  endl;
    }
}

void imprimirTabelaParaArquivo(vector<Termo> termos) {
    for(Termo t : termos){
        output << t.binario <<" 1" <<  endl;
    }
}



//leitura do arquivo .pla
bool lerArquivoPla(const string& nomeArquivo, int& numVariaveis, vector<int>& mintermosDecimais) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cout << "Erro: Nao foi possivel abrir o arquivo " << nomeArquivo << endl;
        return false;
    }

    string linha;
    bool lendoTermos = false;

    while (getline(arquivo, linha)) {
        // Ignora linhas de comentário ou vazias
        if (linha.empty() || linha[0] == '#') {
            continue;
        }

        // Procura por diretivas .i, .p, .e
        if (linha[0] == '.') {
            stringstream ss(linha);
            string token;
            ss >> token;

            if (token == ".i") {
                ss >> numVariaveis;
            } else if (token == ".p") {
                lendoTermos = true; // Indica que as próximas linhas são termos
            } else if (token == ".e") {
                break; // Fim do arquivo
            }
            else{
                lendoTermos = true;
            }
            continue;
        }

        // Se estivermos na seção de termos, processa a linha
        if (lendoTermos) {
            stringstream ss(linha);
            string padraoBinario, padraoSaida;
            ss >> padraoBinario >> padraoSaida;
            
            // Considera apenas termos com saída 1 
            if (padraoSaida == "1") {
                try {
                    // Converte a string binária para um inteiro decimal
                    uint64_t mintermo = stoull(padraoBinario, nullptr, 2);
                    mintermosDecimais.push_back(mintermo);
                } catch (const invalid_argument& e) {
                    cout << "Aviso: Linha com padrao invalido ignorada: " << linha << endl;
                }
            }
        }
    }

    arquivo.close();
    return true;
}


int main(int argc, char* argv[]) {

    cout << "=============================== LATTE ===============================" << endl;
    cout << "        Uma implementação do algoritmo de Quine-McCluskey" << endl;
    cout << "Feito por Beatriz Baroni, Enzo Libório, Gustavo Gomes e Thiago santos" << endl;
    cout << "=====================================================================" << endl;
    cout << endl;

    string primeiroArg = argv[1];

    if(argc < 1){
        cout << "Nenhum arquivo .pla encontrado." << endl;
        cout << "Sintaxe: ./quinemccluskey [nome do arquivo].pla [args] [arquivo de saida]" << endl;
        cout << "Para solicitar ajuda, utilize ./Latte -h";
        return EXIT_FAILURE;
    }

    if(primeiroArg == "-h"){ 
        cout << "Sintaxe: ./Latte [nome do arquivo].pla [args] [arquivo de saida]" << endl;
        cout << "Argumentos:" << endl;
        cout << "W.I.P. (foi mal)" << endl;
        return EXIT_FAILURE;
    }


    
   

    int numVariaveis = 0;
    vector<int> mintermosDecimais;

    
    
    string entrada = argv[1];
    string saida = "";

    if (!lerArquivoPla(entrada, numVariaveis, mintermosDecimais)) {
        return EXIT_FAILURE; // Encerra se houver erro na leitura do arquivo
    }
    

    cout << "Arquivo '" << entrada << "' lido com sucesso." << endl;
    cout << "Numero de variaveis: " << numVariaveis << endl;
    cout << "Mintermos: ";
    for(int m : mintermosDecimais) {
        cout << m << " ";
    }
    cout << endl;

    

 
    vector<Termo> implicantesPrimos;
    vector<vector<Termo>> tabelaDeGrupos(numVariaveis + 1);

    for (uint64_t m : mintermosDecimais) {
        Termo t(m, numVariaveis);
        tabelaDeGrupos[t.contagemDeUns].push_back(t);
    }
    
    while (true) {
        // ... (o restante do algoritmo continua exatamente igual a antes)
        // ...
        bool mudancaFeita = false;
        vector<vector<Termo>> proximaTabelaDeGrupos(numVariaveis + 1);
        set<string> binariosDaProximaTabela;

        for (size_t i = 0; i < tabelaDeGrupos.size() - 1; ++i) {
            for (auto& t1 : tabelaDeGrupos[i]) {
                for (auto& t2 : tabelaDeGrupos[i + 1]) {
                    Termo novoTermo = combinarTermos(t1, t2);
                    if (!novoTermo.binario.empty()) {
                        t1.foiCombinado = true;
                        t2.foiCombinado = true;
                        mudancaFeita = true;
                        
                        if (binariosDaProximaTabela.find(novoTermo.binario) == binariosDaProximaTabela.end()) {
                            int contagemDeUns = 0;
                            for(char c: novoTermo.binario) if(c == '1') contagemDeUns++;
                            novoTermo.contagemDeUns = contagemDeUns;
                            proximaTabelaDeGrupos[novoTermo.contagemDeUns].push_back(novoTermo);
                            binariosDaProximaTabela.insert(novoTermo.binario);
                        }
                    }
                }
            }
        }

        for (const auto& grupo : tabelaDeGrupos) {
            for (const auto& termo : grupo) {
                if (!termo.foiCombinado) {
                    implicantesPrimos.push_back(termo);
                }
            }
        }

        if (!mudancaFeita) break;
        tabelaDeGrupos = proximaTabelaDeGrupos;
    }

    cout << "\n--- Implicantes Primos Encontrados ---\n";
    for(const auto& ip : implicantesPrimos) {
        imprimirTermo(ip);
        cout << "  (cobre m";
        for(uint64_t m : ip.mintermos) cout << m << ",";
        cout << "\b)\n";
    }


    vector<Termo> solucaoFinal;
    set<uint64_t> mintermosACobrir(mintermosDecimais.begin(), mintermosDecimais.end());
    set<uint64_t> indicesImplicantesUsados;

    cout << "\n Selecionando Implicantes Essenciais \n";
    for (int mintermoParaCobrir : mintermosDecimais) {
        int contagemDeCoberturas = 0;
        int ultimoIndiceImplicante = -1;
        for (size_t i = 0; i < implicantesPrimos.size(); ++i) {
            const auto& ip = implicantesPrimos[i];
            if (find(ip.mintermos.begin(), ip.mintermos.end(), mintermoParaCobrir) != ip.mintermos.end()) {
                contagemDeCoberturas++;
                ultimoIndiceImplicante = i;
            }
        }
        if (contagemDeCoberturas == 1 && indicesImplicantesUsados.find(ultimoIndiceImplicante) == indicesImplicantesUsados.end()) {
            solucaoFinal.push_back(implicantesPrimos[ultimoIndiceImplicante]);
            indicesImplicantesUsados.insert(ultimoIndiceImplicante);
            cout << "Mintermo " << mintermoParaCobrir << " e coberto apenas por ";
            imprimirTermo(implicantesPrimos[ultimoIndiceImplicante]);
            cout << ". Adicionado a solucao.\n";
            for (int mintermoCoberto : implicantesPrimos[ultimoIndiceImplicante].mintermos) {
                mintermosACobrir.erase(mintermoCoberto);
            }
        }
    }
    
    cout << "\nCobrindo Mintermos Restantes \n";
    while (!mintermosACobrir.empty()) {
        int melhorIndiceImplicante = -1;
        int maxMintermosCobertos = 0;

        for (size_t i = 0; i < implicantesPrimos.size(); ++i) {
            if (indicesImplicantesUsados.find(i) != indicesImplicantesUsados.end()) continue;

            int mintermosCobertosAtualmente = 0;
            for (int m : implicantesPrimos[i].mintermos) {
                if (mintermosACobrir.count(m)) {
                    mintermosCobertosAtualmente++;
                }
            }
            if (mintermosCobertosAtualmente > maxMintermosCobertos) {
                maxMintermosCobertos = mintermosCobertosAtualmente;
                melhorIndiceImplicante = i;
            }
        }
        
        if (melhorIndiceImplicante != -1) {
            cout << "Selecionado ";
            imprimirTermo(implicantesPrimos[melhorIndiceImplicante]);
            cout << " (cobre " << maxMintermosCobertos << " mintermos restantes).\n";
            
            solucaoFinal.push_back(implicantesPrimos[melhorIndiceImplicante]);
            indicesImplicantesUsados.insert(melhorIndiceImplicante);
            for (int mintermoCoberto : implicantesPrimos[melhorIndiceImplicante].mintermos) {
                mintermosACobrir.erase(mintermoCoberto);
            }
        } else {
            if(!mintermosACobrir.empty()) {
               cout << "ERRO: Nao foi possivel cobrir todos os mintermos.\n";
            }
            break;
        }
    }

    cout << "\nFuncao Minimizada:\nF = ";
    for (size_t i = 0; i < solucaoFinal.size(); ++i) {
        imprimirTermo(solucaoFinal[i]);
        if (i < solucaoFinal.size() - 1) {
            cout << " + ";
        }
    }

    cout << "\n\nTermos da tabela simplificada\n\n";
    imprimirTabela(solucaoFinal);

   

    return 0;
}