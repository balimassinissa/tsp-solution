#include<lpsolve/lp_lib.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<math.h>
#include<stdlib.h>
#include<cstdlib>

using namespace std;
//met toutes les case d'un tableau à zero sauf la premierre
void vider(REAL row[], int nbVariable){
    for(int i = 1 ;i < nbVariable;i++){
        row[i] = 0.0 ;
    }
}
int index(REAL row[],int debut ,int fin ,int x){
    for(int i =debut;i<fin;i++){
        if(row[i]==x) return i;
    }
    return -1;
}
int F2Map(int argc, char *argv[]){
    if(argc != 2){
        printf("Erreur : donnez le chemin du graphe sous le format requis");
        exit(EXIT_FAILURE);
    }
    std::string ligne;
    std::ifstream fichier(argv[1]);
    if(!fichier.is_open()){
        std::cout << "Impossible d'ouvrir le fichier " << std::endl;
        exit(EXIT_FAILURE);
    }
    lprec *lp;
    int nbVilles = 0 ,isAData = 0 , x = 0 ,y = 0 , indexVille = 0;
    REAL positions[1000][2];//le taleau qui va contenir les coordonnées de chaque ville 
         
    while(getline(fichier,ligne))
    {
        string type;
        std::istringstream iss(ligne);
        iss >> type;
        //std::cout << type << std::endl;
             
        if (type == "DIMENSION:"){
            iss >> nbVilles;
        }
        else if(type ==  "NODE_COORD_SECTION"){
            isAData = 1 ;
        }
        else if (type == "EOF"){
            break;
        }  
        else{
            if(isAData == 1){
                iss >> x >> y;
                positions[indexVille][0] = x ;
                positions[indexVille][1] = y ; 
                indexVille += 1 ;
            }
        }
    }
    REAL rowd[nbVilles][nbVilles]={ 0 };//le tableau qui va contenir n colonnes et n lignes dont la case de la colone i et la ligne j represente la distance entre la ville i-1 et j-1
    REAL rowu[nbVilles+1];
    REAL rowx[nbVilles*nbVilles+nbVilles];
    for(int i = 0 ; i < nbVilles ; i++){
        for(int j = 0 ; j < nbVilles ; j++){
            rowd[i][j] = sqrt(pow(positions[i][0] - positions[j][0], 2) + pow(positions[i][1] - positions[j][1], 2));   
         }
    }
    //creation du solver avec les variables
    lp = make_lp(0, nbVilles*nbVilles+nbVilles-1);

    //creation des contraintes

    //xii == 0
    for(int i = 0 ; i<nbVilles;i++){
        vider(rowx,nbVilles*nbVilles+nbVilles);
        rowx[i*nbVilles+i+1] = 1 ;
        add_constraint(lp,rowx,EQ,0);
        
    }
    //variables xij binaires
    for(int i = 0 ; i<nbVilles*nbVilles;i++){
        set_binary(lp,i+1,TRUE);
    }
    //variables ui entieres
    for(int i= nbVilles*nbVilles;i<nbVilles*nbVilles+nbVilles-1;i++){
        set_int(lp,i+1,TRUE);
    }
    //la somme des lignes xij ==0
    for(int i = 0 ;i<nbVilles ; i++){
        vider(rowx,nbVilles*nbVilles+nbVilles);
        for(int j = 0;j<nbVilles;j++){
            rowx[i*nbVilles + j +1] = 1 ;
        }
        add_constraint(lp,rowx,EQ,1);
    }

    //la somme des colonnes xij ==0
    for(int j = 0 ;j<nbVilles ; j++){
        vider(rowx,nbVilles*nbVilles+nbVilles);
        for(int i = 0;i<nbVilles;i++){
            rowx[i*nbVilles + j +1] = 1 ;
        }
        add_constraint(lp,rowx,EQ,1);
    }
    // tout les ui<=n-1 et ui>=1
    for(int i= nbVilles*nbVilles;i<nbVilles*nbVilles+nbVilles-1;i++){
        vider(rowx,nbVilles*nbVilles+nbVilles);
        rowx[i+1]=1;
        add_constraint(lp,rowx,GE,1);
        add_constraint(lp,rowx,LE,nbVilles-1);
    }
    // ui-uj +nxij =<n-1 
    for(int i= nbVilles*nbVilles;i<nbVilles*nbVilles+nbVilles-1;i++){
        
        for(int j= nbVilles*nbVilles;j<nbVilles*nbVilles+nbVilles-1;j++){
            vider(rowx,nbVilles*nbVilles+nbVilles);
            if(j!=i){
                rowx[i+1]=1;
                rowx[j+1]=-1;
                int x =i-nbVilles*nbVilles+1;//le numero de la ville en ligne
                int y =j-nbVilles*nbVilles+1;//le numero de la ville en colone
                rowx[x*nbVilles+y+1]=nbVilles;
                add_constraint(lp,rowx,LE,nbVilles-1);
            }
        }
       
    }
    //la fonction objective
    vider(rowx,nbVilles*nbVilles+nbVilles);
    for(int i=0;i<nbVilles;i++){
        for(int j=0;j<nbVilles;j++){
            rowx[i*nbVilles+j+1] = rowd[i][j];
        }
        
    }
   
    set_obj_fn(lp,rowx);
    if(solve(lp) == 0){
        std::cout <<"la distance minimale est :"<<get_objective(lp) << std::endl;
        get_variables(lp,rowx);
        for(int i = 0 ; i<nbVilles ; i++){
             for(int j = 0 ; j<nbVilles ; j++){
            printf("%.0f ",rowx[i*nbVilles+j]);
            
            }
            printf("\n");
        }
        printf("voici le circuit le plus court :\n");
        printf("ville 1->");
        //generation du fichier dm.dot
        fstream file_out;
        file_out.open("dm.dot", std::ios_base::out);
        if (!file_out.is_open()) {
            cout << "failed to open " << "dm.dot" << '\n';
        } else {
            file_out << "graph {\n" << endl;
        }
        for(int i=1;i<=nbVilles;i++){
            file_out << i<<";\n" << endl;
        }

        int n =1;
        int i = nbVilles*nbVilles;
        int ex = 1 ;
        while(n<nbVilles){
            printf("%d-> ",index(rowx,i,i+nbVilles,n)-nbVilles*nbVilles+2);
           
            file_out << ex<<" -- "<< index(rowx,i,i+nbVilles,n)-nbVilles*nbVilles+2<<";\n"<< endl;
            ex = index(rowx,i,i+nbVilles,n)-nbVilles*nbVilles+2;
            n++;
        }
        file_out << ex<<" -- 1;\n"<< endl;
        file_out <<"}" << endl;
        printf("1");
    }
    else{
         std::cout <<"Pas de solution !!"<< std::endl;
    }
    exit(EXIT_SUCCESS);
}
int main(int argc, char *argv[]){
    F2Map(argc,argv);
    return 0 ;
}