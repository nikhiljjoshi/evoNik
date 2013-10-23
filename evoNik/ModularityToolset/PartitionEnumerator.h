//Original Author: Nicolas Chaumont
#ifndef _PARTITION_ENUMERATOR_H_
#define _PARTITION_ENUMERATOR_H_

#include <vector>

class PartitionEnumerator
{
private:
 unsigned long* Partition; // Partition, which is a list of elements. First entry is the partition size
 size_t network_size; // Self explanatory
 size_t* partitionSize_list; // Partition size before adding a new node
 size_t* usedPaths_list; // List of paths taken at each depth that make up the current partition
 size_t* availablePaths_list; // Number of available paths at a node of a given depth
 size_t* nodePos_list; // Used to remove the node from the right part
 size_t* partitionIncrease_list; // Used to recover the partition size (result[0])
 size_t untouched_nodes; // Number of nodes that don't need processing

 int temp;

 bool isValidPartition(const unsigned long* partition);

public:
#ifdef _MSC_VER
 typedef unsigned __int64 uint64;
#elif __GNUC__
 __extension__ typedef unsigned long long   uint64;
#endif

 PartitionEnumerator(size_t initial_network_size);
 virtual ~PartitionEnumerator();

 uint64 partitionCount();
 void resizeNetwork(size_t new_network_size);
 // ---------------------------------------------------------------------------------------------------------------------------
 // The following functions return a pointer to a valid partition. The format is as follows:
 // - 1st entry: # of elements (E) in the partition.
 // - 'E' next entries: Encoding of the 'E' partition elements (node 'n' is in a partition iff n-th bit is set to 'true').  
 // - Remaining entries: Undefined.
 // ---------------------------------------------------------------------------------------------------------------------------
 const size_t& networkSize();
 const unsigned long* partition();
 void firstPartition();
 void nextPartition();
 void randomPartition(unsigned long* randomPartition);
 void resetPartition(const unsigned long* newPartition);
 void resetPartition(const double* newPartition);
// size_t distance(const unsigned long* first, const unsigned long* second);
};

typedef PartitionEnumerator::uint64 P_uint64;

// -----------------------------------------------------------------------------------------
// Helper functions
// -----------------------------------------------------------------------------------------
// Remap nodes 0, 1,...,map_size to node_map[0], node_map[1],..., node_map[map_size]
// Assumptions:
// -No pointer is NULL
// -First position of P_dest and P_source is the partition size (# of parts)
// -P_dest[0] >= P_source[0]
// -P_dest is initialized to zero (this is actually not necessary, but it makes things simpler)
void remapNodes(unsigned long* P_dest, const unsigned long* P_source, const size_t* node_map, size_t map_size);
// Return the n-th bell number in a 64-bits unsigned integer. Throws if overflow occurs
P_uint64 Bell(size_t network_size);

// -----------------------------------------------------------------------------------------
// Helper macros -- for speed purposes only --
// -----------------------------------------------------------------------------------------
// Remap nodes 0, 1,...,map_size to node_map[0], node_map[1],..., node_map[map_size]
// Assumptions:
// -No pointer is NULL
// -First position of P_dest and P_source is the partition size (# of parts)
// -P_dest[0] >= P_source[0]
// -P_dest is initialized to zero (this is actually not necessary, but it makes things simpler)
#define REMAP_NODES(P_dest, P_source, node_map, map_size) {for(size_t part=0; part<P_source[0]; ++part) for(size_t node=0; node<map_size; ++node) if(P_source[part+1] & (1 << node)) P_dest[part+1] |= (1 << node_map[node]); }
 



#endif

