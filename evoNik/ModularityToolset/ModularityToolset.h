//Original Author: Nicolas Chaumont
#ifndef _PHI_TOOLSET_H_
#define _PHI_TOOLSET_H_

#include <vector>
#include <map>
#include "boost/dynamic_bitset.hpp"
//#include "boost/any.hpp"
//#include "Network.h"

enum E_NORMALIZATION_METHOD
{
    ENM_NONE,
    ENM_TONONI_BALDUZZI,
    ENM_PRODUCT_NORMALIZATION,
    ENM_KOCH,
    ENM_AVG_NODE_DEGREE,
    ENM_COUNT
};

enum E_INPUT_BEHAVIOR        // If the node's input is not connected:
{
    EIB_RETURN_TO_ZERO,        // Assume zero
    EIB_CONSTANT,            // Keep input value from last iteration
    EIB_COUNT                // Number of EIB types 
};

typedef boost::dynamic_bitset<unsigned long>        MT_STATE;
typedef unsigned long                                MT_PART;
typedef std::vector<MT_PART >                        MT_PARTITION;
typedef std::map<size_t, std::vector<MT_STATE > >    MT_TRANSITION_TABLE;
typedef std::map<size_t, size_t>                    MT_FREQUENCIES;
typedef std::map<size_t, MT_FREQUENCIES >            MT_JOINT_FREQUENCIES;
typedef std::map<size_t, double>                    MT_ENTROPIES;

class ModularityToolset
{
public:
    std::map<size_t, size_t>                            part_size;                // Give the number of nodes in the i-th part

    ModularityToolset();
    virtual ~ModularityToolset();

    /*MT_TRANSITION_TABLE feedNetwork(Network& network, 
                                                                    size_t transition_count = 1, 
                                                                    E_INPUT_BEHAVIOR input_behavior = EIB_RETURN_TO_ZERO);
    */
    MT_TRANSITION_TABLE                                 transitionTable(std::istream& in);

    std::vector<size_t>&                                nodeDegree(std::vector<std::vector<double> >& connectivity_matrix);
    void SetAvgNodeDegree(double avgDegree); // JAE Added

    //MT_FREQUENCIES                                        X0frequencies(const MT_TRANSITION_TABLE& transition_table);

    //MT_FREQUENCIES                                        X1frequencies(const MT_TRANSITION_TABLE& transition_table);

    MT_JOINT_FREQUENCIES                                jointX0X1Frequencies(const MT_TRANSITION_TABLE& transition_table);

    std::map<size_t, MT_JOINT_FREQUENCIES >             jointM0M1Frequencies(const MT_TRANSITION_TABLE& transition_table);

    MT_ENTROPIES                                        entropies(const MT_TRANSITION_TABLE& transition_table);

    double                                              entropy(size_t M, const MT_TRANSITION_TABLE& transition_table);

    std::vector<MT_PARTITION >                          MIPs(  const MT_ENTROPIES& entropies, 
                                                               E_NORMALIZATION_METHOD normalization = ENM_NONE);

    std::vector<MT_PARTITION >                          MIPs(   const std::vector<size_t>& subset, 
                                                                const MT_ENTROPIES& entropies, 
                                                                E_NORMALIZATION_METHOD normalization = ENM_NONE);

    std::pair<std::vector<std::vector<size_t> >,double> mainComplexes(  const MT_PARTITION& MIP, 
                                                                        const MT_ENTROPIES& entropies, 
                                                                        E_NORMALIZATION_METHOD normalization = ENM_NONE);

    double                                              ei( const MT_PARTITION& P, 
                                                            E_NORMALIZATION_METHOD normalization,
                                                            const MT_ENTROPIES& entropies);

    double                                              ei( const std::vector<size_t>& subset, 
                                                            const MT_PARTITION& P, 
                                                            E_NORMALIZATION_METHOD normalization,
                                                            const MT_ENTROPIES& entropies);

    // ---------------------------------------------------------------------------
    // Normalization functions
    // ---------------------------------------------------------------------------
    double unNormalized_ei(const unsigned long* partition, size_t node_count, const MT_ENTROPIES& entropies);
    double TononiBalduzzi_ei(const unsigned long* partition, size_t node_count, const MT_ENTROPIES& entropies);
    double product_normalization_ei(const unsigned long* partition, size_t node_count, const MT_ENTROPIES& entropies);
    double Koch_ei(const unsigned long* partition, size_t node_count, const MT_ENTROPIES& entropies);
    double avg_node_degree_ei(const unsigned long* partition, size_t node_count, const MT_ENTROPIES& entropies);


private:
    struct X1_X0_info;
    struct X1_X0_info_PTR_pred;
    struct MU0_PTR_pred;

    std::map<MT_PART, size_t>                        X0_frequency;            // Give F(X0 = x0)
    std::map<MT_PART, size_t>                        X1_frequency;            // Give F(X1 = x1)
    std::map<MT_PART, std::map<MT_PART, size_t> >    X1_and_X0_frequency;    // Give F(X1 = x1, X0 = x0)

    size_t                                           node_count;

    size_t                                                                                   X1_and_X0_count;
    X1_X0_info*                                      X1_and_X0_info;
    X1_X0_info**                                     X1_and_X0_info_PTR;
    X1_X0_info**                                     MU0_info_PTR;

    // ---------------------------------------------------------------------------
    // Node degree-related data structures and functions
    // ---------------------------------------------------------------------------
    bool                                             is_node_degree_preprocessed;    // Flag to keep track of structure initialization
    double                                           avg_node_degree;
    std::vector<size_t>                              node_degree;
        
    // ---------------------------------------------------------------------------
    // Entropy-related data structures and functions
    // ---------------------------------------------------------------------------
    double                              log_2;                      // Speed-up purposes
    bool                                is_entropy_preprocessed;    // Flag to keep track of structure initialization

    void                                preprocessEntropyStructures(const MT_TRANSITION_TABLE& transition_table_PTR);

    void                                cleanUpEntropyStructures();

    std::vector<std::vector<size_t> >    mainComplexes( double& highest_phi,
                                                        size_t last_node_removed,
                                                        const std::vector<size_t> subset,
                                                        const MT_ENTROPIES& H_M0_given_M1,
                                                        E_NORMALIZATION_METHOD normalization);

    void mainComplexes(    
        double& highest_phi,
        std::vector<std::vector<size_t> >& best_subsets, 
        size_t last_node_removed,
        const std::vector<size_t> subset,
        const MT_ENTROPIES& H_M0_given_M1,
        E_NORMALIZATION_METHOD normalization);

    bool isValidPartition(const MT_PARTITION& P);
};

// ---------------------------------------------------------------------------
// Helper functions
// ---------------------------------------------------------------------------
MT_PARTITION        atomicPartition(size_t partition_size);

MT_PARTITION        totalPartition(size_t partition_size);

void                uLong2MT_PARTITION(MT_PARTITION& target, const unsigned long* source);

bool                areSamePartitions(const MT_PARTITION& P1, const MT_PARTITION& P2);

MT_PARTITION        canonicalPartition(const MT_PARTITION& P);

bool                isCanonical(const MT_PARTITION& P);

size_t              partSize(const MT_PART& part, size_t max_size = 32);

size_t              partitionSize(const MT_PARTITION& partition, size_t max_size = 32);

#endif    // _PHI_TOOLSET_H_
