//Original Author: Nicolas Chaumont
#include <cassert>
#include <cmath>
#include <ctime>
#include <cstring>
#include <iostream>
#include <fstream>
#include <set>
#include <algorithm>
#include "boost/dynamic_bitset.hpp"
//#include "boost/any.hpp"
//#include "Node.h"
//#include "Edge.h"
#include "PartitionEnumerator.h"

#include "ModularityToolset.h"
#define JAE_LINE 
#define JAE_VAR(name) 

ModularityToolset::ModularityToolset()
{
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
    log_2 = log(2.0);

    is_node_degree_preprocessed = false;
    is_entropy_preprocessed = false;

    X1_and_X0_count         = 0; 
    X1_and_X0_info          = NULL;
    X1_and_X0_info_PTR      = NULL; 
    MU0_info_PTR            = NULL;
}


ModularityToolset::~ModularityToolset()
{
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
}


MT_TRANSITION_TABLE ModularityToolset::transitionTable(std::istream& in)
{
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
    std::vector<MT_STATE >  X0;
    std::vector<MT_STATE >  X1;
    unsigned long                   val;


    // Load the transition table from the input
    size_t node_count;
    in >> node_count;

    in >> val;
    while(!in.eof())
    {
        X0.push_back(MT_STATE(node_count, val));
        in >> val;
        X1.push_back(MT_STATE(node_count, val));
        in >> val;
    }

    val = X0.size();

    MT_TRANSITION_TABLE transition_table;

    for(size_t state =0; state < val; ++state)
    {
        transition_table[X0[state].to_ulong()].push_back(X1[state]);
    }


    // Initialize the object accordingly
    is_entropy_preprocessed = false;

    return transition_table;
}


std::vector<size_t>&  ModularityToolset::nodeDegree(std::vector<std::vector<double> >& matrix)
{
    assert(matrix.size() > 0);
    node_count = matrix.size();
    assert(matrix[0].size() == node_count);


    avg_node_degree = 0.0;

    node_degree.resize(node_count, 0);

    for(size_t node_1=0; node_1<node_count; node_1++)
    {
        for(size_t node_2=node_1; node_2<node_count; node_2++)
        {
            if(matrix[node_1][node_2] > 0.000000001)
            {
                avg_node_degree++;
                node_degree[node_1]++;
            }
        }
    }

    avg_node_degree /= double(node_count);

    is_node_degree_preprocessed = true;

    return node_degree;
}

void ModularityToolset::SetAvgNodeDegree(double avgDegree) {
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
 }

struct ModularityToolset::X1_X0_info
{

    unsigned long   X0;
    unsigned long   X1;
    unsigned long   mu0;
    unsigned long   mu1;
    size_t                  mu1_mu0_id;
    size_t                  mu0_id;
    size_t                  mu1_id;
        
    size_t                  frequency;
};


struct ModularityToolset::X1_X0_info_PTR_pred
{
    bool operator() (X1_X0_info* i,X1_X0_info* j) 
    { 
        if(i->mu1 != j->mu1)
        {
            return (i->mu1 < j->mu1);
        }
        else
        {
            return (i->mu0 < j->mu0);
        }
    }
};


struct ModularityToolset::MU0_PTR_pred
{
    bool operator() (X1_X0_info* i,X1_X0_info* j) 
    { 
        return (i->mu0 < j->mu0);
    }
};


void ModularityToolset::preprocessEntropyStructures(const MT_TRANSITION_TABLE&  transition_table)
{
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
    size_t                                  X0_count;
    size_t                                  X1_count;

    unsigned long                   x1;

    node_count = transition_table.begin()->second[0].size();
        
    //      ------------------------------------------------------------------------------- //
    //      Initialize the marginal and joint frequencies from the transition table                 //
    //      ------------------------------------------------------------------------------- //
    X0_count = transition_table.size();
        
    X1_frequency.clear();
    X1_and_X0_frequency.clear();

    for(MT_TRANSITION_TABLE::const_iterator x0 = transition_table.begin(); 
        x0!=transition_table.end(); ++x0)
    {
        for(size_t transition=0; transition<x0->second.size(); ++transition)
        {
            x1 = x0->second[transition].to_ulong();

            X1_frequency[x1]++;
            X1_and_X0_frequency[x1][x0->first]++;
        }
    }

    X1_count = X1_frequency.size();


    //      ------------------------------------------------------------------------------- //
    //      Reallocate X1_and_X0_frequency into a plain C array X1_and_X0_freq.
    //      Count the number of entries in X1_and_X0_frequency (stored in X1_and_X0_count).
    //      We'll use 'X1_n_X0_freq_id0' and 'X1_n_X0_freq_id1' to index the array:
    //      
    //      ------------------------------------------------------------------------------- //
    X1_and_X0_count = 0;
    for(std::map<unsigned long, std::map<unsigned long, size_t> >::iterator x1 = X1_and_X0_frequency.begin(); 
        x1 != X1_and_X0_frequency.end(); ++x1)
    {
        X1_and_X0_count         += x1->second.size();
    }

    if(X1_and_X0_info) delete[] X1_and_X0_info;

    X1_and_X0_info = new X1_X0_info[X1_and_X0_count];
        


    if(X1_and_X0_info_PTR) delete[] X1_and_X0_info_PTR;
        
    X1_and_X0_info_PTR = new X1_X0_info*[X1_and_X0_count];



    if(MU0_info_PTR) delete[] MU0_info_PTR;

    MU0_info_PTR = new X1_X0_info*[X1_and_X0_count];
        


    {
        X1_X0_info* info;
        size_t e=0;
        for(std::map<unsigned long, std::map<unsigned long, size_t> >::iterator x1 = X1_and_X0_frequency.begin(); 
            x1 != X1_and_X0_frequency.end(); ++x1)
        {
            for(std::map<unsigned long, size_t>::iterator x0 = x1->second.begin();
                x0 != x1->second.end(); ++x0)
            {
                info                                    = &X1_and_X0_info[e];
                info->X1                                = x1->first;
                info->X0                                = x0->first;
                info->frequency                 = x0->second;
                X1_and_X0_info_PTR[e]   = info;
                MU0_info_PTR[e]                 = info;
                e++;
            }
        }
    }

    is_entropy_preprocessed = true;
}


void ModularityToolset::cleanUpEntropyStructures()
{
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
    // ------------ //
    // Clean-up
    // ------------ //
    if(X1_and_X0_info)
    {               
        delete[] X1_and_X0_info;
        X1_and_X0_info = NULL;
    }
    if(X1_and_X0_info_PTR)
    {       
        delete[] X1_and_X0_info_PTR;
        X1_and_X0_info_PTR = NULL;
    }
    if(MU0_info_PTR)
    {               
        delete[] MU0_info_PTR;
        MU0_info_PTR = NULL;
    }

    is_entropy_preprocessed = false;
}



MT_JOINT_FREQUENCIES ModularityToolset::jointX0X1Frequencies(const MT_TRANSITION_TABLE& transition_table)
{
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
    if(!is_entropy_preprocessed) 
    {
        preprocessEntropyStructures(transition_table);
    }


    MT_JOINT_FREQUENCIES joint_frequencies;


    for(size_t i=0; i<X1_and_X0_count; i++)
    {
        size_t X0        = X1_and_X0_info[i].X0;
        size_t X1        = X1_and_X0_info[i].X1;
        size_t count = X1_and_X0_info[i].frequency;

        if(joint_frequencies.count(X0) && joint_frequencies[X0].count(X1))      joint_frequencies[X0][X1] += count;
        else                                                                                                                            joint_frequencies[X0][X1]  = count;
    }

    return joint_frequencies;
}



std::map<size_t, MT_JOINT_FREQUENCIES > 
ModularityToolset::jointM0M1Frequencies(const MT_TRANSITION_TABLE& transition_table)
{
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
    node_count = transition_table.begin()->second[0].size();

    // Pre-processing if necessary
    if(!is_entropy_preprocessed) preprocessEntropyStructures(transition_table);

    size_t M_state_count    = 1 << node_count;
    // size_t last_M = 0;
        
    std::map<size_t, MT_JOINT_FREQUENCIES > joint_frequencies;

    for(size_t M=1; M<M_state_count; ++M)
    {
        // --------------------------------------------------------     //
        // Map X0s and X1s to mu0s and mu1s for subsequent sorting
        // --------------------------------------------------------     //
        // Update the mu1s and mu0s w.r.t. M
        for(size_t e=0; e<X1_and_X0_count; ++e)
        {
            X1_and_X0_info[e].mu1 = X1_and_X0_info[e].X1 & M;
            X1_and_X0_info[e].mu0 = X1_and_X0_info[e].X0 & M;
        }


        // --------------------------------     //
        // Quick sort the list of pointers
        // --------------------------------     //
        // Sort w.r.t. (mu1, mu0)
        X1_X0_info_PTR_pred X1_X0_pred;
        std::sort(X1_and_X0_info_PTR, X1_and_X0_info_PTR + X1_and_X0_count, X1_X0_pred);
                
        // Sort w.r.t. mu0 only
        MU0_PTR_pred MU0_pred;
        std::sort(MU0_info_PTR, MU0_info_PTR + X1_and_X0_count, MU0_pred);


        // -------------------------------------------------------------------- //
        // Iterate through X1_and_X0_info_PTR to find the # of different mu0s, 
        // mu1s and the # of different (mu1, mu0) pairs.
        // -------------------------------------------------------------------- //
        size_t mu0_count                = 1;    // We count mu0 = 0
        size_t mu1_count                = 1;    // We count mu1 = 0
        size_t mu1_mu0_count    = 1;    // We count (mu1, mu0) = (0, 0)

        unsigned long   last_mu1;
        unsigned long   last_mu0;

        // Count the # of mu1s and (mu1, mu0) pairs
        last_mu1        = X1_and_X0_info_PTR[0]->mu1;
        last_mu0        = X1_and_X0_info_PTR[0]->mu0;

        bool already_incremented = false;
        for(size_t i=0; i<X1_and_X0_count; ++i)
        {
            if(X1_and_X0_info_PTR[i]->mu1 != last_mu1)
            {
                mu1_count++;
                mu1_mu0_count++;
                already_incremented = true;
                                
                last_mu1 = X1_and_X0_info_PTR[i]->mu1;
            }
            if(X1_and_X0_info_PTR[i]->mu0 != last_mu0)
            {
                if(!already_incremented)
                    mu1_mu0_count++;
                                
                last_mu0 = X1_and_X0_info_PTR[i]->mu0;
            }

            X1_and_X0_info_PTR[i]->mu1_id           = mu1_count             - 1;
            X1_and_X0_info_PTR[i]->mu1_mu0_id       = mu1_mu0_count - 1;

            already_incremented = false;
        }



        // -------------------------------------------------------------------- //
        //      Initialize the containers used to compute the entropy
        // -------------------------------------------------------------------- //
        std::map<size_t, double> F_M1;
        std::map<size_t, double> JF_M1_and_M0;  // Pointer to the joint frequencies of M1 and M0.
                
        size_t* M0_M1_to_M1 = new size_t[mu1_mu0_count];
        size_t* M0_M1_to_M0 = new size_t[mu1_mu0_count];



        // Count the # of mu0s
        // Convert joint (M0 M1) frequency index to marginal (M0) frequency index
        last_mu0 = MU0_info_PTR[0]->mu0;
                
        for(size_t i=0; i<X1_and_X0_count; ++i)
        {
            if(MU0_info_PTR[i]->mu0 != last_mu0)
            {
                mu0_count++;

                last_mu0 = MU0_info_PTR[i]->mu0;
            }


            MU0_info_PTR[i]->mu0_id                                         = mu0_count - 1;

            M0_M1_to_M0[MU0_info_PTR[i]->mu1_mu0_id]        = mu0_count - 1;
        }



        // ---------------------------------------------------- //
        // Compute joint and marginal frequencies                               //
        // ---------------------------------------------------- //
        // Loop through all the X0s that transition to X1
        X1_X0_info*     info_PTR;

        for(size_t i = 0; i<X1_and_X0_count;++i)
        {
            info_PTR = &X1_and_X0_info[i];
                        
            // Update the joint frequency and the pointer to the marginal frequency
            JF_M1_and_M0[info_PTR->mu1_mu0_id]      += double(info_PTR->frequency);
                        
            M0_M1_to_M1[info_PTR->mu1_mu0_id]        = info_PTR->mu1_id;
                                        
            F_M1[info_PTR->mu1_id]                          += double(info_PTR->frequency);
        }


        // Everything is computed at this point, so we just save the frequencies
        for(std::map<size_t, double>::iterator i=JF_M1_and_M0.begin();i!=JF_M1_and_M0.end(); i++)
        {
            size_t mu0                      = M0_M1_to_M0[i->first];
            size_t mu1                      = M0_M1_to_M1[i->first];
            size_t frequency        = size_t(i->second);

            joint_frequencies[M][mu0][mu1] += frequency;
        }



        // Clean-up
        delete[] M0_M1_to_M0;
        delete[] M0_M1_to_M1;
    }


    return joint_frequencies;
}



MT_ENTROPIES ModularityToolset::entropies(const MT_TRANSITION_TABLE& transition_table)
{
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
    node_count = transition_table.begin()->second[0].size();

    size_t M_state_count    = 1 << node_count;      // Save the value once for all so that we save calls to vector::size()

    // Decompose each X1 into all possible M1 combinations
    // size_t last_M = 0;
        
    MT_ENTROPIES entropies;

    entropies[0] = node_count;
    for(size_t M=1; M<M_state_count; ++M)   // Already computed in preprocessEntropyStructures
    {
        entropies[M] = entropy(M, transition_table);
    }

    cleanUpEntropyStructures();     
        
    return entropies;
}



double  ModularityToolset::entropy(     size_t M, const MT_TRANSITION_TABLE& transition_table)
{
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
    // --------------------------------------------------------     //
    // Initializations
    // --------------------------------------------------------     //
    size_t          mu1_mu0_count;
    size_t          mu1_count;

    if(part_size.size() != (unsigned int)(1 << node_count))
    {
        part_size.clear();
    }
        
    if(!is_entropy_preprocessed) 
    {
        preprocessEntropyStructures(transition_table);
    }

    // Compute the number of transitions
    size_t transition_count = 0;

    for(MT_TRANSITION_TABLE::const_iterator transition= transition_table.begin();
        transition != transition_table.end(); ++transition)
    {
        transition_count += transition->second.size();
    }

    // --------------------------------------------------------     //
    // Store the part size
    // --------------------------------------------------------     //
    if(part_size[M] == 0)
    {
        for(size_t b=0; b<node_count; ++b)
        {
            if(M & (1 << b))
                ++part_size[M];
        }
    }


    // --------------------------------------------------------     //
    // Map X0s and X1s to mu0s and mu1s for subsequent sorting
    // --------------------------------------------------------     //
    // Update the mu1s and mu0s w.r.t. M
    for(size_t e=0; e<X1_and_X0_count; ++e)
    {
        X1_and_X0_info[e].mu1 = X1_and_X0_info[e].X1 & M;
        X1_and_X0_info[e].mu0 = X1_and_X0_info[e].X0 & M;
    }


    // --------------------------------     //
    // Quick sort the list of pointers
    // --------------------------------     //
    // Sort w.r.t. (mu1, mu0)
    X1_X0_info_PTR_pred X1_X0_pred;
    std::sort(X1_and_X0_info_PTR, X1_and_X0_info_PTR + X1_and_X0_count, X1_X0_pred);
        
    // Sort w.r.t. mu0 only
    MU0_PTR_pred MU0_pred;
    std::sort(MU0_info_PTR, MU0_info_PTR + X1_and_X0_count, MU0_pred);


    // -------------------------------------------------------------------- //
    // Iterate through X1_and_X0_info_PTR to find the # of different mu1s
    // and the # of different (mu1, mu0) pairs.
    // -------------------------------------------------------------------- //
    mu1_count               = 1;    // We count mu1 = 0
    mu1_mu0_count   = 1;    // We count (mu1, mu0) = (0, 0)

    unsigned long   last_mu1        = X1_and_X0_info_PTR[0]->mu1;
    unsigned long   last_mu0        = X1_and_X0_info_PTR[0]->mu0;

    bool already_incremented = false;
    for(size_t i=0; i<X1_and_X0_count; ++i)
    {
        if(X1_and_X0_info_PTR[i]->mu1 != last_mu1)
        {
            mu1_count++;
            mu1_mu0_count++;
            already_incremented = true;
                        
            last_mu1 = X1_and_X0_info_PTR[i]->mu1;
        }
        if(X1_and_X0_info_PTR[i]->mu0 != last_mu0)
        {
            if(!already_incremented)
                mu1_mu0_count++;
                        
            last_mu0 = X1_and_X0_info_PTR[i]->mu0;
        }

        X1_and_X0_info_PTR[i]->mu1_id           = mu1_count             - 1;
        X1_and_X0_info_PTR[i]->mu1_mu0_id       = mu1_mu0_count - 1;

        already_incremented = false;
    }


    // -------------------------------------------------------------------- //
    //      Initialize the containers used to compute the entropy
    // -------------------------------------------------------------------- //
    std::map<size_t, double> F_M1;
    std::map<size_t, double> JF_M1_and_M0;  // Pointer to the joint frequencies of M1 and M0.
    size_t*                                  M0_M1_to_M1;   // Convert joint (M0 M1) frequency index to marginal (M1) frequency index


    M0_M1_to_M1 = new size_t[mu1_mu0_count];

        
    // ---------------------------------------------------- //
    // Compute joint and marginal frequencies                               //
    // ---------------------------------------------------- //
    // Loop through all the X0s that transition to X1
    X1_X0_info*     info_PTR;

    for(size_t i = 0; i<X1_and_X0_count;++i)
    {
        info_PTR = &X1_and_X0_info[i];
                
        // Update the joint frequency and the pointer to the marginal frequency
        JF_M1_and_M0[info_PTR->mu1_mu0_id]      += double(info_PTR->frequency);
                
        M0_M1_to_M1[info_PTR->mu1_mu0_id]        = info_PTR->mu1_id;
                                
        F_M1[info_PTR->mu1_id]                          += double(info_PTR->frequency);
    }


    // ------------------------------------ //
    // Compute entropies from frequencies   //
    // ------------------------------------ //
    double entropy = 0.0;

    for(size_t i=0; i<mu1_mu0_count; ++i)
    {
        assert(F_M1.count(M0_M1_to_M1[i]));
        assert(JF_M1_and_M0.count(i));

        entropy -= JF_M1_and_M0[i] / double(transition_count) * log(JF_M1_and_M0[i] / F_M1[M0_M1_to_M1[i]]) / log_2;
    }


    // -------------------- //
    // Clean-up and return
    // -------------------- //
    delete[] M0_M1_to_M1;

    return entropy;
}



std::vector<MT_PARTITION > ModularityToolset::MIPs(const MT_ENTROPIES& entropies,
                                                   E_NORMALIZATION_METHOD normalization)
{
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
    node_count = entropies.find(0)->second;

    // Compute part_size
    for(MT_ENTROPIES::const_iterator it=entropies.begin(); it!=entropies.end(); ++it)
    {
        part_size[it->first] = partSize(it->first, node_count);
    }

    PartitionEnumerator             enumerator(node_count);
    P_uint64                                partition_count = enumerator.partitionCount();
    const unsigned long*    partition       = enumerator.partition();
      //*source = partition + 1;
    MT_PARTITION                    empty_MIP(node_count, 0);
        
    std::vector<MT_PARTITION > equivalent_MIPs(1, empty_MIP);

    // Initialize the minimum information
    double  min_information;

    memcpy(&equivalent_MIPs.back()[0], partition+1, node_count * sizeof(unsigned long));

    min_information = node_count;

    // Compute the minimum information partition
    // P_uint64 last_p = 0;
    for(P_uint64 p=1; p<partition_count; ++p)
    {
        // Proceed to the next partition
        enumerator.nextPartition();

        // Compute its integrated information
        double ei_norm;

        switch (normalization)
        {
        case ENM_KOCH:
            ei_norm = Koch_ei(partition, node_count, entropies);
            break;
        case ENM_TONONI_BALDUZZI:
            ei_norm = TononiBalduzzi_ei(partition, node_count, entropies);
            break;
        case ENM_PRODUCT_NORMALIZATION:
            ei_norm = product_normalization_ei(partition, node_count, entropies);
            break;
        case ENM_AVG_NODE_DEGREE:
            JAE_LINE;
            JAE_VAR(is_node_degree_preprocessed);
            JAE_VAR(this);
            ei_norm = avg_node_degree_ei(partition, node_count, entropies);
            break;
        case ENM_NONE:
            ei_norm = unNormalized_ei(partition, node_count, entropies);
            break;
        default:
            std::cerr << "Error in ModularityToolset::MIP(): Unknown normalization method ";
            std::cerr << normalization << ". Returning -1.0." << std::endl;
            ei_norm =  -1.0;
        }

        // --------------------------------------------------------------------------------     //
        // Keep track of the minimum information partition (MIP):
        //      - If the new partition has the same value, store it with the rest
        //      - If the new partition is significantly smaller, clear the vector and store it
        // --------------------------------------------------------------------------------     //
        double difference = ei_norm - min_information;
                
        if(fabs(difference) < 0.0000000001)
        {
            equivalent_MIPs.push_back(empty_MIP);
            memcpy(&equivalent_MIPs.back()[0], partition+1, node_count * sizeof(unsigned long));
        }
        else if(difference < 0)
        {       
            equivalent_MIPs.clear();
            equivalent_MIPs.resize(1, empty_MIP);

            memcpy(&equivalent_MIPs.back()[0], partition+1, node_count * sizeof(unsigned long));
            min_information = ei_norm;
        }
    }

    return equivalent_MIPs;
}


std::vector<MT_PARTITION >      ModularityToolset::MIPs(const std::vector<size_t>& subset, 
                                                        const MT_ENTROPIES& entropies, 
                                                        E_NORMALIZATION_METHOD normalization)
{
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
    // 1- Use the subset of nodes to reconstruct a reduced H_M0_given_M1
    MT_ENTROPIES reduced_entropies;

    size_t subset_size = subset.size();

    reduced_entropies[0] = subset_size;
    for(size_t M=1; M< (unsigned int)(1 << subset_size); M++)
    {
        size_t mapped_M = 0;

        for(size_t bit=0; bit<subset_size; bit++)
        {
            if(M & (1 << bit))
            {
                mapped_M |= (1 << subset[bit]);
            }
        }

        reduced_entropies[M] = entropies.find(mapped_M)->second;
    }

    // 2- Use the MIPs algorithm to find the MIPs
    ModularityToolset toolset;
    if (is_node_degree_preprocessed) { // JAE FIX
        toolset.SetAvgNodeDegree(avg_node_degree);
    }
    std::vector<MT_PARTITION > equivalent_subset_mips = toolset.MIPs(reduced_entropies, normalization);

    // 3- Relocate the nodes so that we get (reduced) partitions with valid parts
    for(size_t mip=0; mip<equivalent_subset_mips.size(); mip++)
    {
        size_t subset_mip_size = partitionSize(equivalent_subset_mips[mip], subset_size);

        for(size_t part=0; part<subset_mip_size; part++)
        {
            MT_PART current_part = 0;

            for(size_t node=0; node<subset_size; node++)
            {
                if(equivalent_subset_mips[mip][part] & (1 << node))
                {
                    // Add the node at its new location
                    current_part |= (1 << subset[node]);
                }
            }

            equivalent_subset_mips[mip][part] = current_part;
        }

        // 3.1- Resize the partition so that it has the right number of nodes (add trailing zeros)
        equivalent_subset_mips[mip].resize(entropies.find(0)->second);
    }
        
    // 4- Return the solution
    return equivalent_subset_mips;
}


std::pair<std::vector<std::vector<size_t> >,double> ModularityToolset::mainComplexes(   
    const MT_PARTITION& MIP,
    const MT_ENTROPIES& entropies,
    E_NORMALIZATION_METHOD normalization)
{
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
    std::vector<size_t> subset(MIP.size());

    for(size_t node=0; node<subset.size(); node++)
    {
        subset[node] = node;
    }

    double highest_phi = -1.0;

    //return mainComplexes(highest_phi, 0, subset, H_M0_given_M1, normalization);

    std::vector<std::vector<size_t> > best_subsets;

    mainComplexes(highest_phi, best_subsets, 0, subset, entropies, normalization);

    return std::pair<std::vector<std::vector<size_t> >,double>(best_subsets, highest_phi);
}


void ModularityToolset::mainComplexes(  
    double& highest_phi,
    std::vector<std::vector<size_t> >& best_subsets,
    size_t last_node_removed,
    const std::vector<size_t> subset, //JAE: Why is this passed by value? Why not a reference?
    const MT_ENTROPIES& entropies,
    E_NORMALIZATION_METHOD normalization)
{
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
    // This is what we'll return in the end
    std::vector<std::vector<size_t> > main_complexes;

        
    // 1- Use the subset of nodes to reconstruct a reduced H_M0_given_M1
    MT_ENTROPIES reduced_entropies;

    size_t total_size       = size_t(entropies.find(0)->second);
    size_t subset_size      = subset.size();
    JAE_LINE;
    reduced_entropies[0] = subset_size;
    for(size_t M=1; M< (unsigned int)(1 << subset_size); M++)
    {
        size_t mapped_M = 0;

        for(size_t bit=0; bit<subset_size; bit++)
        {
            if(M & (1 << bit))
            {
                mapped_M |= (1 << subset[bit]);
            }
        }

        reduced_entropies[M] = entropies.find(mapped_M)->second;
    }
    JAE_LINE;
        
    // 2- Compute the MIPs
    ModularityToolset toolset;
    if (is_node_degree_preprocessed) { // JAE FIX
        toolset.SetAvgNodeDegree(avg_node_degree);
    }
    std::vector<MT_PARTITION > equivalent_mips = toolset.MIPs(reduced_entropies, normalization);

        
    // 3- Compute <Phi>
    double Phi = highest_phi - 1.0;
        
    if(equivalent_mips.size()) Phi = fabs(toolset.ei(equivalent_mips[0], ENM_NONE, reduced_entropies));


    // 4- Update <Phi> and the equivalent_mips if necessary
    if(Phi > highest_phi) // JAE changed from >= per Nicolas' email
    {
        // If phi is higher, get rid of the existing subsets
        if(fabs(Phi - highest_phi) >= 0.000001)
        {
            best_subsets.clear();
        }

        best_subsets.push_back(subset);

        highest_phi = Phi;
    }

    // 5- Recursively look for better complexes from the current set of nodes
    for(size_t n=last_node_removed; n<total_size; n++)
    {
        // 5.1- Create a new subset without node 'n'
        std::vector<size_t> new_subset = subset;

        for(size_t node=0; node<new_subset.size(); node++)
        {
            if(new_subset[node] == n)
            { 
                new_subset.erase(new_subset.begin() + node);
                break;
            }
        }

        // 5.2- Compute the main complex -- ignore single nodes
        if(new_subset.size() > 1)
        {
            mainComplexes(highest_phi, best_subsets, n+1, new_subset, entropies, normalization);
        }
    }
}


std::vector<std::vector<size_t> > ModularityToolset::mainComplexes(     
    double& highest_phi,
    size_t last_node_removed,
    const std::vector<size_t> subset,
    const MT_ENTROPIES& H_M0_given_M1,
    E_NORMALIZATION_METHOD normalization)
{
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
    // This is what we'll return in the end
    std::vector<std::vector<size_t> > main_complexes;

        
    // 1- Use the subset of nodes to reconstruct a reduced H_M0_given_M1
    MT_ENTROPIES reduced_entropies;

    size_t total_size       = size_t(H_M0_given_M1.find(0)->second);
    size_t subset_size      = subset.size();

    reduced_entropies[0] = subset_size;
    for(size_t M=1; M<(unsigned int)(1 << subset_size); M++)
    {
        size_t mapped_M = 0;

        for(size_t bit=0; bit<subset_size; bit++)
        {
            if(M & (1 << bit))
            {
                mapped_M |= (1 << subset[bit]);
            }
        }

        reduced_entropies[M] = H_M0_given_M1.find(mapped_M)->second;
    }

        
    // 2- Compute the MIPs
    ModularityToolset toolset;
    if (is_node_degree_preprocessed) { // JAE FIX
        toolset.SetAvgNodeDegree(avg_node_degree);
    }
    std::vector<MT_PARTITION > equivalent_mips = toolset.MIPs(reduced_entropies, normalization);

        
    // 2- Compute <Phi>
    double Phi = highest_phi - 1.0;
        
    if(equivalent_mips.size()) Phi = fabs(toolset.ei(equivalent_mips[0], ENM_NONE, reduced_entropies));


    // 3- Update <Phi> and the equivalent_mips if necessary
    if(Phi >= highest_phi)
    {
        highest_phi = Phi;

        std::vector<size_t> part_subset;

        // Recursively look for better complexes from the current set of nodes
        double previous_highest_phi = highest_phi;

        for(size_t n=last_node_removed; n<total_size; n++)
        {
            // Create a new subset without node 'n'
            std::vector<size_t> new_subset = subset;

            for(size_t node=0; node<new_subset.size(); node++)
            {
                if(new_subset[node] == n)
                { 
                    new_subset.erase(new_subset.begin() + node);
                    break;
                }
            }

            // Ignore single nodes
            if(new_subset.size() > 1)
            {
                std::vector<std::vector<size_t> > better_complexes = mainComplexes(highest_phi, n+1, new_subset, H_M0_given_M1, normalization);

                // If better complexes are found, 2 things can happen:
                //      1- The complexes have same Phi as the ones found in other subsets: concatenate them together
                //      2- The complexes have higher Phi: Replace the previous ones with the new ones
                if(better_complexes.size())
                {
                    //      1- The complexes have same Phi as the ones found in other subsets: concatenate them together
                    if(fabs(highest_phi - previous_highest_phi) < 0.000001)
                    {
                        for(size_t complex=0; complex<better_complexes.size(); complex++)
                        {
                            main_complexes.push_back(better_complexes[complex]);
                        }
                    }
                    //      2- The complexes have higher Phi: Replace the previous ones with the new ones
                    else
                    {
                        main_complexes = better_complexes;
                    }
                }
            }
        }

        // If no better complexes have been found, use the one passed as parameter
        if(!main_complexes.size())
        {
            main_complexes.resize(1);
            main_complexes[0] = subset;
        }
    }
    // No need to do anything if the main_complexes is empty: couldn't find better complexes

    return main_complexes;
}


double ModularityToolset::ei(const MT_PARTITION &partition, 
                             E_NORMALIZATION_METHOD normalization,
                             const MT_ENTROPIES& entropies)
{
    assert(isValidPartition(partition));
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
    node_count = size_t(entropies.find(0)->second);


    size_t partition_size = partitionSize(partition, partition.size());

        
    // --------------------------------------------------------     //
    // Store the part size
    // --------------------------------------------------------     //
    for(size_t part=0; part<partition_size; part++)
    {
        size_t M = partition[part];

        if(part_size[M] == 0)
        {
            for(size_t b=0; b<node_count; ++b)
            {
                if(M & (1 << b))
                    ++part_size[M];
            }
        }
    }


    MT_PARTITION P(partition);                      // Create a non-const partition
    P.insert(P.begin(), partition_size);// Insert the partition size at the beginning (low-level format)

    switch (normalization)
    {
    case ENM_KOCH:
        return Koch_ei(&P[0], node_count, entropies);
    case ENM_TONONI_BALDUZZI:
        return TononiBalduzzi_ei(&P[0], node_count, entropies);
    case ENM_PRODUCT_NORMALIZATION:
        return product_normalization_ei(&P[0], node_count, entropies);
    case ENM_AVG_NODE_DEGREE:
        JAE_LINE;
        JAE_VAR(is_node_degree_preprocessed);
        JAE_VAR(this);
        return avg_node_degree_ei(&P[0], node_count, entropies);
    case ENM_NONE:
        return unNormalized_ei(&P[0], node_count, entropies);
    default:
        std::cerr << "Error in ModularityToolset::ei(): Unknown normalization method ";
        std::cerr << normalization << ". Returning -1.0." << std::endl;
        return -1.0;
    }
}



double ModularityToolset::ei(   const std::vector<size_t>& subset, 
                                const MT_PARTITION& P, 
                                E_NORMALIZATION_METHOD normalization,
                                const MT_ENTROPIES& entropies)
{
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
    // 1- Use the subset of nodes to reconstruct a reduced H_M0_given_M1
    MT_ENTROPIES reduced_entropies;

    size_t subset_size = subset.size();

    reduced_entropies[0] = subset_size;
    for(size_t M=1; M< (unsigned int)(1 << subset_size); M++)
    {
        size_t mapped_M = 0;

        for(size_t bit=0; bit<subset_size; bit++)
        {
            if(M & (1 << bit))
            {
                mapped_M |= (1 << subset[bit]);
            }
        }

        reduced_entropies[M] = entropies.find(mapped_M)->second;
    }

        
    // 2- Remove entries in the partition that do not belong to the subset.
    //      P can have trailing zeros that correspond to nodes in excess of those in the subset
    //      The nodes in P that are not part of the subset are useless, so we can remove them.
    // size_t initial_node_count = P.size();

    MT_PARTITION reduced_P(P);      // Enough nodes, not at the right position, too many zeros
        
    reduced_P.resize(subset_size);  // Remove zeros in excess

    // Reposition the zeros
    size_t mip_size = partitionSize(P, subset_size);

    for(size_t part=0; part<mip_size; part++)
    {
        MT_PART current_part = 0;

        for(size_t node=0; node<subset_size; node++)
        {
            if(reduced_P[part] & (1 << subset[node]))
            {
                // Copy the node at its new location
                current_part |= (1 << node);
            }
        }

        reduced_P[part] = current_part;
    }
        

    // 3- Use the <ei> algorithm to find <ei> and return the solution
    ModularityToolset toolset;
    if (is_node_degree_preprocessed) { // JAE FIX
        toolset.SetAvgNodeDegree(avg_node_degree);
    }
    return toolset.ei(reduced_P, normalization, reduced_entropies);
}


// ---------------------------------------------------------------------------
// Heuristic-related functions
// ---------------------------------------------------------------------------
bool ModularityToolset::isValidPartition(const MT_PARTITION& P)
{
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
    size_t accum = 0;

    for(size_t part=0; part<P.size(); part++)
    {
        accum += P[part];
    }

    return (accum == (unsigned int)((1 << P.size()) - 1));
}

// ---------------------------------------------------------------------------
// Normalization functions:
// ---------------------------------------------------------------------------
//      - unNormalized_ei
//      - unNormalized_total_partition_ei
//      - TononiBalduzzi_ei
//      - product_normalization_ei
//      - TononiBalduzzi_total_partition_ei
//      - Adami_ei
//      - QN
// ---------------------------------------------------------------------------
double ModularityToolset::unNormalized_ei(const unsigned long* partition, size_t node_count, const MT_ENTROPIES& entropies)
{
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
    double _ei;

    size_t k = partition[0];

    if(k == 1)
    {
        _ei = node_count - entropies.find((1 << node_count) - 1)->second;
    }
    else
    {
        _ei     = -entropies.find((1 << node_count) - 1)->second;
                 
        for(size_t part=0; part < k; ++part)
        {
            _ei += entropies.find(partition[part+1])->second;
        }       
    }

    return _ei;
}


double ModularityToolset::TononiBalduzzi_ei(const unsigned long* partition, size_t node_count, const MT_ENTROPIES& entropies)
{
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
    size_t k = partition[0];
    double _ei;

    if(k == 1)
    {
        _ei = (node_count - entropies.find((1 << node_count) - 1)->second) / node_count;
    }
    else
    {
        size_t smallest_part    = node_count;

        _ei     = -entropies.find((1 << node_count) - 1)->second;
                
        // Compute the smaller part (smaller number of nodes) in the partition
        for(size_t part=0; part < k; ++part)
        {
            _ei += entropies.find(partition[part+1])->second;

            // Update the minimum number of nodes if necessary
            size_t p_size = part_size[partition[part+1]];

            if(p_size == 0) 
                p_size = part_size[partition[part+1]] = partSize(partition[part+1], node_count);


            if(smallest_part > p_size) smallest_part = p_size;
        }

        _ei /= ((k - 1) * smallest_part);
    }

    return _ei;
}



double ModularityToolset::product_normalization_ei(const unsigned long* partition, size_t node_count, const MT_ENTROPIES& entropies)
{
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
    size_t k = partition[0];

    double _ei;

    if(k == 1)
    {
        _ei = entropies.find(0)->second;
    }
    else
    {
        _ei     = -entropies.find((1 << node_count) - 1)->second;
        size_t accum    = 1;

        // Compute the smaller part (smaller number of nodes) in the partition
        for(size_t part=0; part < k; ++part)
        {
            _ei += entropies.find(partition[part+1])->second;

            // Update the product normalization
            accum *= part_size[partition[part+1]];
        }

        _ei /= accum;
    }

    return _ei;
}



// Koch normalization - assume you you have m pieces of size S_i in the MIP. 
// The total size of the network is n = \sum_i S_i
// Then N_Koch= -\sum_i p_i *log(p_i)
// where p_i = S_i/n
double ModularityToolset::Koch_ei(const unsigned long* partition, size_t node_count, const MT_ENTROPIES& entropies)
{
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
    double _ei;

    if(partition[0] == 1)
    {
        _ei = entropies.find(0)->second;
    }
    else
    {
        _ei = unNormalized_ei(partition, node_count, entropies);

        double Koch_denominator = 0;

        for(size_t part=0; part<partition[0]; part++)
        {
            size_t p_size = part_size[partition[part+1]];

            if(p_size == 0) 
                p_size = part_size[partition[part+1]] = partSize(partition[part+1], node_count);

            // p_i = S_i/n
            double p = double(p_size) / double(node_count);

            // N_Koch= -\sum_i p_i *log(p_i)
            Koch_denominator -= p * log(p) / log_2;
        }

        _ei /= Koch_denominator;
    }


    return _ei;
}


//This C++ code is supposed to emulate the following code in Python from Tononi's lab:
//def norm(partition,n,avg): #the function to calculate normalization values
//    a=[]
//    for i in range(len(partition)):
//            a.append(len(partition[i]))
//
//    # important part of the norm-algorithm:
//    # determine the largest part and test whether 
//    # at least 1/avg of it can be covered by the remaining parts
//    # if not: calculate leftovers and substract from n
//    # if they can: normalization is n
//    a.sort() 
//    if (a[len(a) - 1] > avg * sum(a[:len(a) - 1])):
//        return (n - (a[len(a) - 1] - avg*sum(a[:len(a) - 1])))
//    else:
//        return n
// Where partition can be [[0,1,3,4,5],[2]]
// n is the number of nodes in the network
// avg is the average node degree
double ModularityToolset::avg_node_degree_ei(const unsigned long* partition, size_t node_count, const MT_ENTROPIES& entropies)
{
    JAE_LINE;
    JAE_VAR(is_node_degree_preprocessed);
    JAE_VAR(this);
    if(is_node_degree_preprocessed)
    {
        double _ei;

        if(partition[0] == 1)
        {
            _ei = entropies.find(0)->second;
        }
        else
        {
            // Find the largest part and store its index so that:
            // largest_part == part_size[partition[largest_part_id]]
            // Also, compute avg_part_size
            size_t largest_part = 0;
            size_t largest_part_id;
            double avg_part_size = 0;

            for(size_t part=0; part<partition[0]; part++)
            {
                // Check that the size of the parts are already computed. This is equivalent to the python code:
                //a=[]
                //for i in range(len(partition)):
                //              a.append(len(partition[i]))

                size_t p_size = part_size[partition[part+1]];

                if(p_size == 0) 
                    p_size = part_size[partition[part+1]] = partSize(partition[part+1], node_count);

                // Check part sizes:
                // - if the current part size is the largest, store its size and ID
                // - if not, add the current part's contribution to 'avg_part_size'
                // It is equivalent to doing simultaneously the following two lines in Python:
                //a.sort() and pick the largest a[len(a) - 1]
                //avg * sum(a[:len(a) - 1])

                if(largest_part < p_size)
                {
                    // Add the contribution from the previous largest_part (that was skipped) to avg_part_size:
                    avg_part_size += double(largest_part) * avg_node_degree;

                    // Update largest part information and don't add it to 'avg_part_size'
                    largest_part = p_size;
                    largest_part_id = part;
                }
                else
                {
                    avg_part_size += double(p_size) * avg_node_degree;
                }
            }

            // Initialize _ei
            _ei = unNormalized_ei(partition, node_count, entropies);

            // This code is exactly equivalent to the 4 lines of the C++ code below
            _ei /= (largest_part > avg_part_size)?(partition[0] + avg_part_size - largest_part):(partition[0]);

                        
            // Those lines are the exact translation of the original python code from Tononi's lab:
            //
            // ---- Python code:
            //if (a[len(a) - 1] > avg * sum(a[:len(a) - 1])):
            //      return (n - (a[len(a) - 1] - avg*sum(a[:len(a) - 1])))
            //else:
            //      return n
            //
            // ---- C++ code:
            //double denominator = 0;
            //if(largest_part > avg_part_size)      denominator = partition[0] - (largest_part - avg_part_size);
            //else                                                          denominator = partition[0];
            //_ei /= denominator;
        }

        return _ei;
    }
    else
    {
        std::cerr << "Error (ModularityToolset::avg_node_degree_ei): Node degree information is not available." << std::endl;
        std::cerr << "Use ModularityToolset::nodeDegree to compute your network's average node degree." << std::endl;
        std::cerr << "avg_node_degree=" <<avg_node_degree<< std::endl;
        throw "Missing degree information";
    }
}



// ---------------------------------------------------------------------------
// Helper functions
// ---------------------------------------------------------------------------
//      - atomicPartition
//      - totalPartition
//      - uLong2MT_PARTITION
//      - areSamePartitions
//      - areOneNodeAway
//      - canonicalPartition
// ---------------------------------------------------------------------------
MT_PARTITION atomicPartition(size_t partition_size)
{
    assert(partition_size <= 32);

    MT_PARTITION partition(partition_size);

    for(size_t part=0; part<partition_size; part++)
    {
        partition[part] = (1 << part);
    }

    return partition;
}


MT_PARTITION totalPartition(size_t partition_size)
{
    assert(partition_size <= 32);

    MT_PARTITION partition(partition_size, 0);

    partition[0] = (1 << (partition_size)) - 1;

    return partition;
}


void uLong2MT_PARTITION(MT_PARTITION& target, const unsigned long* source)
{
    size_t accum = 0;
    target.resize(source[0]);
        
    for(size_t part=0; part<source[0]; ++part)
    {
        target[part] = source[part + 1];
        accum           += source[part + 1];
    }

    size_t minimum_shift = source[0];
        
    while((((accum << minimum_shift) >> minimum_shift) == accum) && (minimum_shift < 32))
    {
        minimum_shift++;
    }
        
    target.resize(32 - minimum_shift + 1, 0);
}


bool areSamePartitions( const MT_PARTITION& P1, const MT_PARTITION& P2)
{
    if(P1.size() == P2.size())
    {
        for(size_t part = 0; part < P1.size(); ++part)
        {
            if(P1[part] != P2[part]) return false;
        }
        return true;
    }
    else
    { 
        return false;
    }
}


MT_PARTITION    canonicalPartition(const MT_PARTITION& P)
{
    MT_PARTITION result;

    if(!isCanonical(P))
    {
        MT_PARTITION temp(P);

        result.resize(P.size(), 0);

        size_t non_empty_parts = 0;

        // Count the non-empty parts in P
        for(size_t part=0; part<P.size(); ++part)
        {
            if(P[part])     non_empty_parts++;
            else            break;
        }

        // Reorder the partitions
        MT_PARTITION::iterator it;
        for(size_t part=0; part<non_empty_parts; ++part)
        {
            it = std::min_element(temp.begin(), temp.begin() + (non_empty_parts - part));

            result[part] = *it;

            temp.erase(it);
        }

        return result;
    }


    return result = P;      
}


bool isCanonical(const MT_PARTITION& P)
{
    bool is_canonical = true;
    size_t node_count = P.size();

    for(size_t part=1; part<node_count; ++part)
    {
        if(P[part])
        {
            is_canonical &= P[part-1] < P[part];
        }
        else
        {
            break;
        }
    }

    return is_canonical;
}


size_t  partSize(const MT_PART& part, size_t max_size)
{
    size_t part_size = 0;

    for(size_t bit=0; bit<max_size; bit++)
    {
        if(part & (1 << bit))
            part_size++;
    }

    return part_size;
}


size_t  partitionSize(const MT_PARTITION& partition, size_t max_size)
{
    size_t partition_size = 0;

    for(size_t part=0; part<max_size; part++)
    {
        if(partition[part])     partition_size++;
        else break;
    }

    return partition_size;
}


