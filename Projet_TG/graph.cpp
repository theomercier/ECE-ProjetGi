#include "graph.h"

/************************************************************************************************************************************************************************************************************
                                            VERTEX
*************************************************************************************************************************************************************************************************************/

/// Le constructeur met en place les �l�ments de l'interface
VertexInterface::VertexInterface(int idx, int x, int y, std::string pic_name, int pic_idx)
{
    // La boite englobante
    m_top_box.set_pos(x, y);
    m_top_box.set_dim(130, 100);
    m_top_box.set_moveable();

    // Le slider de r�glage de valeur
    m_top_box.add_child( m_slider_value );
    m_slider_value.set_range(0.0, 100.0);  // Valeurs arbitraires, � adapter...
    m_slider_value.set_dim(20,80);
    m_slider_value.set_gravity_xy(grman::GravityX::Left, grman::GravityY::Up);

    // Label de visualisation de valeur
    m_top_box.add_child( m_label_value );
    m_label_value.set_gravity_xy(grman::GravityX::Left, grman::GravityY::Down);

    // Une illustration...
    if (pic_name!="")
    {
        m_top_box.add_child( m_img );
        m_img.set_pic_name(pic_name);
        m_img.set_pic_idx(pic_idx);
        m_img.set_gravity_x(grman::GravityX::Right);
    }

    // Label de visualisation d'index du sommet dans une boite
    m_top_box.add_child( m_box_label_idx );
    m_box_label_idx.set_gravity_xy(grman::GravityX::Right, grman::GravityY::Down);
    m_box_label_idx.set_dim(20,12);
    m_box_label_idx.set_bg_color(BLANC);

    m_box_label_idx.add_child( m_label_idx );
    m_label_idx.set_message( std::to_string(idx) );
}


/// Gestion du Vertex avant l'appel � l'interface
void Vertex::pre_update()
{
    if (!m_interface)
        return;

    /// Copier la valeur locale de la donn�e m_value vers le slider associ�
    m_interface->m_slider_value.set_value(m_value);

    /// Copier la valeur locale de la donn�e m_value vers le label sous le slider
    m_interface->m_label_value.set_message( std::to_string( (int)m_value) );
}


/// Gestion du Vertex apr�s l'appel � l'interface
void Vertex::post_update()
{
    if (!m_interface)
        return;

    /// Reprendre la valeur du slider dans la donn�e m_value locale
    m_value = m_interface->m_slider_value.get_value();
}

void Vertex::displayVertex()
{
    std::cout<<m_indice_sommet << " " << m_value <<" " <<m_pop<<" "<<std::endl;
}


/************************************************************************************************************************************************************************************************************
                                            EDGE
*************************************************************************************************************************************************************************************************************/

/// Le constructeur met en place les �l�ments de l'interface
EdgeInterface::EdgeInterface(Vertex& from, Vertex& to)
{
    // Le WidgetEdge de l'interface de l'arc
    if ( !(from.m_interface && to.m_interface) )
    {
        std::cerr << "Error creating EdgeInterface between vertices having no interface" << std::endl;
        throw "Bad EdgeInterface instanciation";
    }
    m_top_edge.attach_from(from.m_interface->m_top_box);
    m_top_edge.attach_to(to.m_interface->m_top_box);
    m_top_edge.reset_arrow_with_bullet();

    // Une boite pour englober les widgets de r�glage associ�s
    m_top_edge.add_child(m_box_edge);
    m_box_edge.set_dim(24,60);
    m_box_edge.set_bg_color(BLANCBLEU);

    // Le slider de r�glage de valeur
    m_box_edge.add_child( m_slider_weight );
    m_slider_weight.set_range(0.0, 100.0);  // Valeurs arbitraires, � adapter...
    m_slider_weight.set_dim(16,40);
    m_slider_weight.set_gravity_y(grman::GravityY::Up);

    // Label de visualisation de valeur
    m_box_edge.add_child( m_label_weight );
    m_label_weight.set_gravity_y(grman::GravityY::Down);

}


/// Gestion du Edge avant l'appel � l'interface
void Edge::pre_update()
{
    if (!m_interface)
        return;

    /// Copier la valeur locale de la donn�e m_weight vers le slider associ�
    m_interface->m_slider_weight.set_value(m_weight);

    /// Copier la valeur locale de la donn�e m_weight vers le label sous le slider
    m_interface->m_label_weight.set_message( std::to_string( (int)m_weight ) );
}

/// Gestion du Edge apr�s l'appel � l'interface
void Edge::post_update()
{
    if (!m_interface)
        return;

    /// Reprendre la valeur du slider dans la donn�e m_weight locale
    m_weight = m_interface->m_slider_weight.get_value();
}

void Edge::display_edge()
{
    std::cout << m_from << " " << m_to << " " << m_weight << std::endl;
}


/************************************************************************************************************************************************************************************************************
                                            GRAPH
**********************************************************************************************************************************************************/



void Graph::init(int path)
{
    std::string pathfolder,pathpic;

    m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);

    load_graphPOP(path);
    load_graph(path);
    //displayAlleg(path);
}

/// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Ici le constructeur se contente de pr�parer un cadre d'accueil des
/// �l�ments qui seront ensuite ajout�s lors de la mise ne place du Graphe
GraphInterface::GraphInterface(int x, int y, int w, int h)
{
    m_top_box.set_dim(1000,740);
    m_top_box.set_gravity_xy(grman::GravityX::Right, grman::GravityY::Up);

    m_top_box.add_child(m_tool_box);
    m_tool_box.set_dim(80,720);
    m_tool_box.set_gravity_xy(grman::GravityX::Left, grman::GravityY::Up);
    m_tool_box.set_bg_color(BLANCBLEU);

    m_top_box.add_child(m_main_box);
    m_main_box.set_dim(908,720);
    m_main_box.set_gravity_xy(grman::GravityX::Right, grman::GravityY::Up);
    m_main_box.set_bg_color(BLANCJAUNE);
}



///Chargement////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///M�thode pour lire les fichiers et le load dans les diff�rents conteneurs
void Graph::load_graph(int fic)
{
    std::string fic_name;
    std::vector<int> temp(m_ordre,0);

    if(fic==1)
    {
        fic_name ="Matrice_P/savane.txt";
        m_ordre = 20;
    }
    else if(fic==2)
    {

    }
    else if(fic==3)
    {

    }
    else
    {
        std::cout << "erreur fatale";
        exit(1);
    }

    for(int i(0); i<m_ordre; i++)
    {
        m_matP.push_back(temp);
    }

    std::ifstream fichier(fic_name, std::ios::in);

    if(!fichier.is_open())  // si l'ouverture echoue
    {
        std::cout << "Erreur � l'ouverture de la matrice d'adjacence !" << std::endl;
    }

    for(int i(0); i<m_ordre ; i++)
    {
        for(int j(0); j<m_ordre ; j++)
        {
            fichier >>  m_matP[i][j];
        }
    }

    fichier.close();
}

///chargement du graph population
void Graph::load_graphPOP(int fic)
{
    std::string fic_name;
    int temp;

    if(fic==1)
    {
        fic_name ="Population/savane.txt";
        m_ordre=20;
    }
    else if(fic==2)
    {

    }
    else if(fic==3)
    {

    }
    else
    {
        std::cout << "erreur fatale";
        exit(1);
    }

    std::ifstream fichier(fic_name, std::ios::in);

    if(!fichier.is_open())  // si l'ouverture echoue
    {
        std::cout << "Erreur � l'ouverture du tableau des populations !" << std::endl;
    }

    for(int i(0); i<m_ordre ; i++)
    {
        fichier >>  temp;
        m_matPOP.push_back(temp);
    }

    fichier.close();
}

///supression dess sommets

void Graph::del_vertex()
{
    int temp;


    if(key[KEY_D])
    {

        int fin(0);
        std::cout<<"Indice du sommet pr�sent sur le graph a supprimer : "<<std::endl;
        std::cin>>temp;

        Vertex &remed =m_vertices.at(temp);
        std::cout<< "removing vertex "<< temp <<" "<< remed.m_pop <<std::endl;

        if(m_interface && remed.m_interface)
        {

            for(int i(0);i<m_vertices[temp].m_in.empty();i++)
            {
                std::cout<<i<<"top"<<std::endl;
                test_remove_edge(m_vertices[temp].m_in[i]);
            }

            for(int i(0);i<m_vertices[temp].m_out.empty();i++)
            {
                std::cout<<i<<"top1"<<std::endl;
                test_remove_edge(m_vertices[temp].m_out[i]);
            }

            //m_edges.erase()



            //std::cout<<temp;
            m_interface->m_main_box.remove_child(remed.m_interface->m_top_box);
            m_vertices.erase(temp);

            /*for(int i(0);i<m_vertices[temp].m_in.empty();i++)
            {
                test_remove_edge(m_vertices[temp].m_in[i]);
            }

            for(int i(0);i<m_vertices[temp].m_out.empty();i++)
            {
                test_remove_edge(m_vertices[temp].m_out[i]);
            }*/
        }
    }
}



/// eidx index of edge to remove
void Graph::test_remove_edge(int eidx)
{
    /// r�f�rence vers le Edge � enlever
    Edge &remed=m_edges.at(eidx);
    std::cout << "Removing edge " << eidx << " " << remed.m_from << "->" << remed.m_to << " " << remed.m_weight << std::endl;

    /*/// Tester la coh�rence : nombre d'arc entrants et sortants des sommets 1 et 2
    std::cout << m_vertices[remed.m_from].m_in.size() << " " << m_vertices[remed.m_from].m_out.size() << std::endl;
    std::cout << m_vertices[remed.m_to].m_in.size() << " " << m_vertices[remed.m_to].m_out.size() << std::endl;
    std::cout << m_edges.size() << std::endl;*/

    std::cout<<"pre boucle  ";
    /// test : on a bien des �l�ments interfac�s
    if (m_interface && remed.m_interface)
    {
        std::cout<<"boucle  ";
        /// Ne pas oublier qu'on a fait �a � l'ajout de l'arc :
        /* EdgeInterface *ei = new EdgeInterface(m_vertices[id_vert1], m_vertices[id_vert2]); */
        /* m_interface->m_main_box.add_child(ei->m_top_edge); */
        /* m_edges[idx] = Edge(weight, ei); */
        /// Le new EdgeInterface ne n�cessite pas de delete car on a un shared_ptr
        /// Le Edge ne n�cessite pas non plus de delete car on n'a pas fait de new (s�mantique par valeur)
        /// mais il faut bien enlever le conteneur d'interface m_top_edge de l'arc de la main_box du graphe
        m_interface->m_main_box.remove_child( remed.m_interface->m_top_edge );
    }

    /// Il reste encore � virer l'arc supprim� de la liste des entrants et sortants des 2 sommets to et from !
    /// References sur les listes de edges des sommets from et to
    std::vector<int> &vefrom = m_vertices[remed.m_from].m_out;
    std::vector<int> &veto = m_vertices[remed.m_to].m_in;
    vefrom.erase( std::remove( vefrom.begin(), vefrom.end(), eidx ), vefrom.end() );
    veto.erase( std::remove( veto.begin(), veto.end(), eidx ), veto.end() );

    /// Le Edge ne n�cessite pas non plus de delete car on n'a pas fait de new (s�mantique par valeur)
    /// Il suffit donc de supprimer l'entr�e de la map pour supprimer � la fois l'Edge et le EdgeInterface
    /// mais malheureusement ceci n'enlevait pas automatiquement l'interface top_edge en tant que child de main_box !
    m_edges.erase( eidx );

    /// Tester la coh�rence : nombre d'arc entrants et sortants des sommets 1 et 2
    std::cout << m_vertices[remed.m_from].m_in.size() << " " << m_vertices[remed.m_from].m_out.size() << std::endl;
    std::cout << m_vertices[remed.m_to].m_in.size() << " " << m_vertices[remed.m_to].m_out.size() << std::endl;
    std::cout << m_edges.size() << std::endl;
}
///DISPLAY////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///affichage du graph en console
void Graph::show_graph_console()
{
    for(int i(0); i<m_ordre ; i++)
    {
        for(int j(0); j<m_ordre ; j++)
        {
            std::cout<< m_matP[i][j]<<" ";
        }
        std::cout<<std::endl;
    }
}

///affichage graph pop en console
void Graph::show_graph_consolePOP()
{
    for(int i(0); i<m_ordre ; i++)
    {
        std::cout<< m_matPOP[i]<<" ";
    }
}

///Display des sommets
void Graph::display_vertices()
{
    for(int i(0); i<m_ordre; i++)
    {
        m_vertices[i].displayVertex();
    }

    std::cout<<std::endl;
    std::cout<<std::endl;
    std::cout<<std::endl;
}

///Affichage des arretes
void Graph::display_edges()
{
    for(int i(0); i<m_nbedges; i++)
    {
        m_edges[i].display_edge();
    }
    std::cout<<std::endl;
    std::cout<<std::endl;
    std::cout<<std::endl;

    //std::cout<<m_nbedges;
}



/// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// La m�thode update � appeler dans la boucle de jeu pour les graphes avec interface
void Graph::update()
{
    if (!m_interface)
        return;

    for (auto &elt : m_vertices)
        elt.second.pre_update();

    for (auto &elt : m_edges)
        elt.second.pre_update();

    m_interface->m_top_box.update();

    for (auto &elt : m_vertices)
        elt.second.post_update();

    for (auto &elt : m_edges)
        elt.second.post_update();
}



/// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Aide � l'ajout de sommets interfac�s
void Graph::add_interfaced_vertex(int idx, double value, int x, int y, std::string pic_name, int pic_idx )
{
    ///on verifie si le ssommet n'existe pa deja , si c'est la cas on ne fait rien
    if ( m_vertices.find(idx)!=m_vertices.end() )
    {
        std::cerr << "Error adding vertex at idx=" << idx << " already used..." << std::endl;
    }
    else///ici on ajoute le sommet et on ajoute ses arretes si il en a avec les sommets deja pr�sents
    {
        // Cr�ation d'une interface de sommet
        VertexInterface *vi = new VertexInterface(idx, x, y, pic_name, pic_idx);
        // Ajout de la top box de l'interface de sommet
        m_interface->m_main_box.add_child(vi->m_top_box);
        // On peut ajouter directement des vertices dans la map avec la notation crochet :
        m_vertices[idx] = Vertex(idx,0,value, vi);

        for (int j(0); j<m_ordre; j++)
        {
            if ((m_matP[idx][j] != 0))
            {
                //std::cout<<j<<"coucou c'est j"<<std::endl;
                //std::cout<<idx<<"coucou c'est idx"<<std::endl;
                add_interfaced_edge(m_nbedges,idx,j,m_matP[idx][j]);
            }
            else if((m_matP[j][idx] != 0))
            {
               add_interfaced_edge(m_nbedges,j,idx,m_matP[idx][j]);
            }
        }
    }

    for(const auto& elem:m_vertices[idx].m_in)
    {
        std::cout << elem << " sommets qui in  m_in de "<<idx <<std::endl;
    }

    for(const auto& elem:m_vertices[idx].m_out)
    {
        std::cout << elem << " sommets qui out de m_out de   "<<  idx <<std::endl;
    }
}

/// Aide � l'ajout d'arcs interfac�s
void Graph::add_interfaced_edge(int idx, int id_vert1, int id_vert2, double weight)
{
    int ok=0;

    if ( m_edges.find(idx)!=m_edges.end() )
    {
        std::cerr << "Error adding edge at idx=" << idx << " already used..." << std::endl;
        ok=1;
    }

    if ( m_vertices.find(id_vert1)==m_vertices.end() || m_vertices.find(id_vert2)==m_vertices.end() )
    {
        //std::cerr << "Error adding edge idx=" << idx << " between vertices " << id_vert1 << " and " << id_vert2 << " not in m_vertices" << std::endl;
        ok=1;
    }

    if(ok==0)
    {

        //m_interface->m_main_box.remove_child( remed.m_interface->m_top_edge );

        EdgeInterface *ei = new EdgeInterface(m_vertices[id_vert1], m_vertices[id_vert2]);
        m_interface->m_main_box.add_child(ei->m_top_edge);
        m_edges[idx] = Edge(id_vert1, id_vert2, weight, ei);

        /// OOOPS ! Prendre en compte l'arc ajout� dans la topologie du graphe !

        m_edges[idx].m_from = id_vert1;
        m_edges[idx].m_to = id_vert2;

        m_vertices[id_vert1].m_out.push_back(id_vert2);
        m_vertices[id_vert2].m_in.push_back(id_vert1);

        //std::cout<<m_nbedges<<std::endl;
        m_nbedges++;
    }
}

void Graph::add_vertex(int path)
{
    int temp(-1);

    if(key[KEY_H])
    {
        std::cout<<"Indice de votre sommet ? ";
        std::cin>> temp;
        std::cout<< std::endl;

        if(temp>=0 && temp<=19)
        {
            add_interfaced_vertex(temp,getPop(temp),100,100,getPicName(temp,path));
        }
    }
}


/// Sauvegarde//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Graph::save(int path)
{
    savePOP(path);
    //saveCoef();
    //savePOS();
}

void Graph::savePOP(int path)
{
    std::string ficName;

    if(key[KEY_D])
    {
        if(path==1)
        {
            ficName ="Population/savane.txt";
        }
        else if(path==2)
        {

        }
        else if(path==3)
        {

        }


        std::ofstream fichier(ficName, std::ios::out | std::ios::trunc);

        if(!fichier)
        {
            std::cout<<"erreur enregistrement POP"<<std::endl;
        }

        for(int i(0); i<m_ordre; i++)
        {

        }
    }
}

void Graph::saveCoef()
{

}

void Graph::savePOS()
{


}



///   GETTERs//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vertex Graph::getVertex(int t)
{
    return m_vertices[t];
}

int Graph::getPop(int t)
{
    return m_matPOP[t];
}

std::string Graph::getPicName(int idx, int path)
{
    std::string name;

    if(path==1)
    {
        name="pics/savane/" + std::to_string(idx) + ".jpg" ;
    }
    return name;
}

