#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include <commun.h>
#include <liste.h>
#include <piste.h>




int 
main( int nb_arg , char * tab_arg[] )
{     

  int cle_piste ;
  piste_t * piste = NULL ;

  liste_t * liste = NULL;

  int cle_liste ;
  /* liste_t * liste = NULL ; */

  char marque ;

  booleen_t fini = FAUX ;
  piste_id_t deplacement = 0 ;
  piste_id_t depart = 0 ;
  piste_id_t arrivee = 0 ;

  
  cell_t cell_cheval ;


  elem_t elem_cheval ;

  int shmid_liste;



  /*-----*/

  if( nb_arg != 4 )
    {
      fprintf( stderr, "usage : %s <cle de piste> <cle de liste> <marque>\n" , tab_arg[0] );
      exit(-1);
    }
  
  if( sscanf( tab_arg[1] , "%d" , &cle_piste) != 1 )
    {
      fprintf( stderr, "%s : erreur , mauvaise cle de piste (%s)\n" , 
	       tab_arg[0]  , tab_arg[1] );
      exit(-2);
    }


  if( sscanf( tab_arg[2] , "%d" , &cle_liste) != 1 )
    {
      fprintf( stderr, "%s : erreur , mauvaise cle de liste (%s)\n" , 
	       tab_arg[0]  , tab_arg[2] );
      exit(-2);
    }

  if( sscanf( tab_arg[3] , "%c" , &marque) != 1 )
    {
      fprintf( stderr, "%s : erreur , mauvaise marque de cheval (%s)\n" , 
	       tab_arg[0]  , tab_arg[3] );
      exit(-2);
    }
     

  /* Init de l'attente */
  commun_initialiser_attentes() ;


  /* Init de la cellule du cheval pour faire la course */
  cell_pid_affecter( &cell_cheval  , getpid());
  cell_marque_affecter( &cell_cheval , marque );

  /* Init de l'element du cheval pour l'enregistrement */
  elem_cell_affecter(&elem_cheval , cell_cheval ) ;
  elem_etat_affecter(&elem_cheval , EN_COURSE ) ;

  /* 
   * Enregistrement du cheval dans la liste
   */
  int id_piste = atoi(tab_arg[1]);
  int id_liste = atoi(tab_arg[2]);


  //liste = malloc(LISTE_MAX * sizeof(int));

  

  
  printf("arggggss %d \n",id_liste);


  

  shmid_liste = shmget(id_liste,LISTE_MAX * sizeof(int),0666);
  if (shmid_liste == -1)
  {
    perror("Erreur de lecture : ");
  }
  
  printf("shmid_liste %d \n",shmid_liste);

  
  liste_t * attach = shmat(shmid_liste,liste,0);
  if (attach == (int*)-1)
  {
    perror("association adr echoue : ");
  }

  

  liste_elem_ajouter(attach,elem_cheval);

  printf("nb de la liste %d \n",attach->nb);


  printf("sem de chev : %d",elem_cheval.sem);

  elem_sem_creer(attach->liste);

  int semid = elem_sem_lire(*attach->liste);

  printf("sem id equals %d \n",semid);

  elem_sem_detruire(attach->liste);
  

  liste_elem_supprimer(attach,1);




  //elem_sem_creer(liste->liste);
  //elem_sem_verrouiller(liste);
  //liste_elem_affecter(liste->liste, liste_nb_lire,elem_cheval);
  //elem_sem_deverrouiller(liste);

  //printf("Liste pointeur : %p",liste);




  

  while( ! fini )
    {
      /* Attente entre 2 coup de de */
      commun_attendre_tour() ;
   
      /* 
       * Verif si pas decanille 
       */

      if (!elem_decanille(elem_cheval))
      {

      }



      /*
       * Avancee sur la piste
       */

      /* Coup de de */
      deplacement = commun_coup_de_de() ;
#ifdef _DEBUG_
      printf(" Lancement du De --> %d\n", deplacement );
#endif

      arrivee = depart+deplacement ;

      if( arrivee > PISTE_LONGUEUR-1 )
	{
	  arrivee = PISTE_LONGUEUR-1 ;
	  fini = VRAI ;
	}

      if( depart != arrivee )
	{

	  /* 
	   * Si case d'arrivee occupee alors on decanille le cheval existant 
	   */

	       
	  /* 
	   * Deplacement: effacement case de depart, affectation case d'arrivee 
	   */

#ifdef _DEBUG_
	  printf("Deplacement du cheval \"%c\" de %d a %d\n",
		 marque, depart, arrivee );
#endif

	  
	} 
      /* Affichage de la piste  */
      piste_afficher_lig( piste );
     
      depart = arrivee ;
    }

  printf( "Le cheval \"%c\" A FRANCHIT LA LIGNE D ARRIVEE\n" , marque );

 
     
  /* 
   * Suppression du cheval de la liste 
   */

  
  exit(0);
}











