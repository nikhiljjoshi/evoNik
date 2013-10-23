//Original Author: Nicolas Chaumont
#include <cassert>
#include <iostream>
#include <cstring> // for memcpy
#include <cstdlib> // for rand
#include "PartitionEnumerator.h"


PartitionEnumerator::PartitionEnumerator(size_t initial_network_size)
{
    network_size = initial_network_size;
    
    Partition = new unsigned long[network_size + 1];
    Partition[0] = 1;
    for(size_t pos=1; pos<network_size+1; ++pos) Partition[pos] = 0;
    
    partitionSize_list = new size_t[network_size];
    partitionSize_list[0] = 1;
    
    usedPaths_list = new size_t[network_size];
    for(size_t p=0; p<network_size; ++p) usedPaths_list[p] = 0;
    
    nodePos_list = new size_t[network_size];
    for(size_t pos=0; pos<network_size; ++pos) nodePos_list[pos] = 1;
    
    partitionIncrease_list = new size_t[network_size];
    
    partitionIncrease_list[0] = 1;
    for(size_t pos=1; pos<network_size; ++pos) partitionIncrease_list[pos] = 0;
    
    availablePaths_list = new size_t[network_size];
    
    untouched_nodes = 0;
    
    // The partition is undefined. Let's position ourselves on the first partition
    firstPartition();
}


PartitionEnumerator::~PartitionEnumerator()
{
    if(Partition) delete[] Partition;
    if(partitionSize_list) delete[] partitionSize_list;
    if(usedPaths_list) delete[] usedPaths_list;
    if(nodePos_list) delete[] nodePos_list;
    if(partitionIncrease_list) delete[] partitionIncrease_list;
    if(availablePaths_list) delete[] availablePaths_list;
}


PartitionEnumerator::uint64 PartitionEnumerator::partitionCount()
{
    return Bell(network_size);
}


void PartitionEnumerator::resizeNetwork(size_t new_network_size)
{
    if(network_size != new_network_size)
    {
        network_size = new_network_size;
        
        if(Partition)
        { 
            delete[] Partition;
            Partition = NULL;
        }
        Partition = new unsigned long[network_size+1];
        Partition[0] = 1;
        for(size_t pos=1; pos<network_size+1; ++pos) Partition[pos] = 0;
        
        
        if(partitionSize_list)
        { 
            delete[] partitionSize_list;
            partitionSize_list = NULL;
        }
        partitionSize_list = new size_t[network_size];
        partitionSize_list[0] = 1;
        
        
        if(usedPaths_list)
        { 
            delete[] usedPaths_list;
            usedPaths_list = NULL;
        }
        usedPaths_list = new size_t[network_size];
        for(size_t p=0; p<network_size; ++p) usedPaths_list[p] = 0;
        
        
        if(nodePos_list)
        { 
            delete[] nodePos_list;
            nodePos_list = NULL;
        }
        nodePos_list = new size_t[network_size];
        for(size_t pos=0; pos<network_size; ++pos) nodePos_list[pos] = 1;
        
        
        if(partitionIncrease_list)
        { 
            delete[] partitionIncrease_list;
            partitionIncrease_list = NULL;
        }
        partitionIncrease_list = new size_t[network_size];
        
        partitionIncrease_list[0] = 1;
        for(size_t pos=1; pos<network_size; ++pos) partitionIncrease_list[pos] = 0;
        
        
        if(availablePaths_list)
        { 
            delete[] availablePaths_list;
            availablePaths_list = NULL;
        }
        availablePaths_list = new size_t[network_size];
        
        untouched_nodes = 0;
    }
    
    // The partition is undefined. Let's position ourselves on the first partition
    firstPartition();
}


const size_t& PartitionEnumerator::networkSize()
{
    return network_size;
}


const unsigned long* PartitionEnumerator::partition()
{
    return Partition;
}


void PartitionEnumerator::firstPartition()
{
    // Initialize the partition:
    Partition[0] = 1; // Partition size is 1
    Partition[1] = (1 << network_size) - 1; // One single part with all the nodes in it
    
    // Initialize the partition size after addition
    for(size_t n=0; n<network_size; ++n)
        partitionSize_list[n] = 1;
    
    // Initialize the partition increase history
    partitionIncrease_list[0] = 1; // Increased by 1 the first time
    memset(&partitionIncrease_list[0] + 1, 0, sizeof(unsigned long) * (network_size-1)); // No subsequent increase
    
    // Initialize the node addition history (all additions at pos zero)
    for(size_t n=0; n<network_size; ++n)
        nodePos_list[n] = 1;
    
    // Initialize the partition size after addition
    availablePaths_list[0] = 1;
    for(size_t n=1; n<network_size; ++n)
        availablePaths_list[n] = 2;
    
    // Initialize the partition size after addition
    for(size_t n=0; n<network_size-1; ++n)
        usedPaths_list[n] = 0;
    usedPaths_list[network_size - 1] = 1;
    
    // Only 1 node to modify next time
    untouched_nodes = network_size - 1;
}


void PartitionEnumerator::nextPartition()
{
    // Revert the outdated modifications made to the partition
    for(size_t d=untouched_nodes; d<network_size; ++d)
    {
        temp = (int) (network_size - 1 - d + untouched_nodes);
        Partition[nodePos_list[temp]] &= ~(1 << temp); // Remove the current node
        Partition[0] -= partitionIncrease_list[temp]; // Update partition size
    }
    
    // All modifications are relevant to the current partition. We now can add nodes to it
    for(size_t d=untouched_nodes; d<network_size; ++d)
    {
        if(d) availablePaths_list[d] = partitionSize_list[d-1] + 1;
        else availablePaths_list[d] = 1;
        
        // Should we add a part to this partition?
        if(usedPaths_list[d] == availablePaths_list[d]-1)
        { 
            if(d) partitionSize_list[d] = partitionSize_list[d-1] + 1;
            else partitionSize_list[d] = 1;
            
            // Add new node to the new part
            // Equivalent to partition.push_back(1 << d)
            Partition[0]++;
            Partition[Partition[0]] = 1 << d;
            
            // Track the changes in the partition so that we can revert them later
            partitionIncrease_list[d] = 1; // The partition grew by 1
            nodePos_list[d] = Partition[0]; // Record where the node was added
            untouched_nodes++; // We have a new valid hierarchical level
        }
        // Just need to add the new node to the d-th part
        else
        {
            if(d) partitionSize_list[d] = partitionSize_list[d-1];
            else partitionSize_list[d] = 1;
            
            // Add the new node to an existing part
            Partition[usedPaths_list[d]+1] |= (1 << d);
            
            // Track the changes in the partition so that we can revert them later
            partitionIncrease_list[d] = 0; // The partition hasn't grown
            nodePos_list[d] = usedPaths_list[d]+1; // Record where the node was added
            untouched_nodes++; // We have a new valid hierarchical level
        }
        
        
        // We reached a leaf, increment the number of elements we've added
        if(d == network_size-1)
        {
            usedPaths_list[d]++;
            untouched_nodes--;
        }
        
        // If the current depth is saturated, then find the next branch in the enumeration tree 
        // that is not saturated
        if(usedPaths_list[d] == availablePaths_list[d])
        {
            // -----------------------------------------------------------------------------------------------
            // The current depth is saturated, i.e. no more partition can be enumerated from this parent depth. 
            // We have to 'roll back' in the enumeration tree to the next node that has partitions left 
            // to enumerate.
            // -----------------------------------------------------------------------------------------------
            size_t offset = 0;
            // Go up the hierarchy stepwise
            while((usedPaths_list[d-offset] == availablePaths_list[d-offset]) && (d != offset))
            {
                // Clear changes made to the current node
                usedPaths_list[d-offset]--; // Reset the part list locus to a legal value
                untouched_nodes--; // This node should be reassigned
                
                // Update the next level to allow cascading updates if necessary
                usedPaths_list[d-offset-1]++;
                
                offset++;
            }
            
            // If path went up to the root, we are at the last partition. Correct the first node so that
            // the usedPaths_list remains valid if the user wants this partition again
            if(d == offset)
            {
                usedPaths_list[0] = 0;
            }
            // If we didn't reach the root, then we found a node from which to enumerate. We just have to
            // reset those entries in usedPaths_list that correspond to the depths below that node.
            else
            {
                for(size_t depth_to_reset=d-offset+1; depth_to_reset<network_size; ++depth_to_reset)
                {
                    usedPaths_list[depth_to_reset] = 0;
                }
            }
        }
    }
}


// random_partition is of size 'PartitionEnumerator::network_size'
// randomPartition is initialized with zeros
void PartitionEnumerator::randomPartition(unsigned long* random_partition)
{
    // Code this
    size_t path;
    random_partition[0] = 1;
    random_partition[1] = 1;
    
    for(size_t depth=0; depth<network_size-1; ++depth)
    {
        path = rand() % (random_partition[0] + 1);
        
        if(path == random_partition[0]) ++random_partition[0];
        
        random_partition[path] |= (1 << (depth + 1));
    }
}


void PartitionEnumerator::resetPartition(const unsigned long* newPartition)
{
    assert(isValidPartition(newPartition));
    
    untouched_nodes = network_size - 1;
    
    // Copy and pre-process each part
    Partition[0] = newPartition[0];
    
    for(size_t p=1; p<=newPartition[0]; ++p)
    {
        Partition[p] = newPartition[p];
        
        // Look for the nodes within a part.
        for(size_t n=0; n<network_size; ++n)
        {
            // If node 'n' belongs to this part, store this information in 'nodePos_list'
            if(Partition[p] & (1 << n))
            {
                nodePos_list[n] = p; // Node was found in the d-th part
            }
        }
    }
    // Complete the processing on each node.
    partitionSize_list[0] = 1; // Partition has 1 part (i.e. size = 1)
    partitionIncrease_list[0] = 1; // Partition has increased by 1 (from zero to 1)
    availablePaths_list[0] = 1; // Two paths available from first node
    usedPaths_list[0] = nodePos_list[0]-1;// Path is node position - 1
    
    for(size_t d=1; d<network_size; ++d)
    {
        availablePaths_list[d] = partitionSize_list[d-1] + 1;
        usedPaths_list[d] = nodePos_list[d]-1;
        
        // New node belongs to an existing part
        if((nodePos_list[d] - 1) < partitionSize_list[d-1])
        {
            partitionSize_list[d] = partitionSize_list[d-1];
            partitionIncrease_list[d] = 0;
        }
        // New node belongs to a new part. Create it
        else
        {
            partitionSize_list[d] = partitionSize_list[d-1] + 1;
            partitionIncrease_list[d] = 1;
        }
        
        // We reached a leaf, increment the number of elements we've added
        if(d == network_size-1)
        {
            usedPaths_list[d]++;
            untouched_nodes--;
        }
        
        // If the current depth is saturated, then find the next branch in the enumeration tree 
        // that is not saturated
        if(usedPaths_list[d] == availablePaths_list[d])
        {
            // -----------------------------------------------------------------------------------------------
            // The current depth is saturated, i.e. no more partition can be enumerated from this parent depth. 
            // We have to 'roll back' in the enumeration tree to the next node that has partitions left 
            // to enumerate.
            // -----------------------------------------------------------------------------------------------
            size_t offset = 0;
            // Go up the hierarchy stepwise
            while((usedPaths_list[d-offset] == availablePaths_list[d-offset]) && (d != offset))
            {
                // Clear changes made to the current node
                usedPaths_list[d-offset]--; // Reset the part list locus to a legal value
                untouched_nodes--; // This node should be reassigned
                
                // Update the next level to allow cascading updates if necessary
                usedPaths_list[d-offset-1]++;
                
                offset++;
            }
            
            // If path went up to the root, we are at the last partition. Correct the first node so that
            // the usedPaths_list remains valid if the user wants this partition again
            if(d == offset)
            {
                usedPaths_list[0] = 0;
            }
            // If we didn't reach the root, then we found a node from which to enumerate. We just have to
            // reset those entries in usedPaths_list that correspond to the depths below that node.
            else
            {
                for(size_t depth_to_reset=d-offset+1; depth_to_reset<network_size; ++depth_to_reset)
                {
                    usedPaths_list[depth_to_reset] = 0;
                }
            }
        }
    }
}


void PartitionEnumerator::resetPartition(const double* newPartition)
{
    untouched_nodes = network_size - 1;
    
    // Initialization for the first node
    partitionSize_list[0] = 1; // Partition has 1 part (i.e. size = 1)
    Partition[0] = 1; // Partition has size 1 (contains node zero)
    Partition[1] = 1; // First part contains node 1
    nodePos_list[0] = 1; // First node is always in the first part
    partitionIncrease_list[0] = 1; // Partition has increased by 1 (from zero to 1)
    availablePaths_list[0] = 1; // Two paths available from first node, which index grunt up to 1
    
    size_t used_path = 0;
    
    for(size_t d=1; d<network_size; ++d)
    {
        // Proceed to the next path
        availablePaths_list[d] = partitionSize_list[d-1] + 1;
        used_path = (size_t)(newPartition[d-1] * (double)(availablePaths_list[d]));
        
        usedPaths_list[d] = used_path;
        
        // Path-dependent initializations
        if(used_path < partitionSize_list[d-1])
        {
            partitionSize_list[d] = partitionSize_list[d-1];
            partitionIncrease_list[d] = 0;
        }
        else
        {
            Partition[0]++;
            partitionSize_list[d] = partitionSize_list[d-1] + 1;
            partitionIncrease_list[d] = 1;
        }
        
        // Common initializations
        Partition[used_path + 1]   |= (1 << d);
        nodePos_list[d] = used_path + 1;
        
        // We reached a leaf, increment the number of elements we've added
        if(d == network_size-1)
        {
            usedPaths_list[d]++;
            // untouched_nodes--;
        }
        
        // If the current depth is saturated, then find the next branch in the enumeration tree 
        // that is not saturated
        if(usedPaths_list[d] == availablePaths_list[d])
        {
            // -----------------------------------------------------------------------------------------------
            // The current depth is saturated, i.e. no more partition can be enumerated from this parent depth. 
            // We have to 'roll back' in the enumeration tree to the next node that has partitions left 
            // to enumerate.
            // -----------------------------------------------------------------------------------------------
            size_t offset = 0;
            // Go up the hierarchy stepwise
            while((usedPaths_list[d-offset] == availablePaths_list[d-offset]) && (d != offset))
            {
                // Clear changes made to the current node
                usedPaths_list[d-offset]--; // Reset the part list locus to a legal value
                untouched_nodes--; // This node should be reassigned
                
                // Update the next level to allow cascading updates if necessary
                usedPaths_list[d-offset-1]++;
                
                offset++;
            }
            
            // If path went up to the root, we are at the last partition. Correct the first node so that
            // the usedPaths_list remains valid if the user wants this partition again
            if(d == offset)
            {
                usedPaths_list[0] = 0;
            }
            // If we didn't reach the root, then we found a node from which to enumerate. We just have to
            // reset those entries in usedPaths_list that correspond to the depths below that node.
            else
            {
                for(size_t depth_to_reset=d-offset+1; depth_to_reset<network_size; ++depth_to_reset)
                {
                    usedPaths_list[depth_to_reset] = 0;
                }
            }
        }
    }
}


//size_t PartitionEnumerator::distance(const unsigned long* first, const unsigned long* last)
//{
// size_t longest_partition = (std::max)(first[0], last[0]);
//
// // Compare the partition positions:
// // - If first is before last, we proceed
// // - If first == last, we return zero distance
// // - If first is after last, we return 0 distance (might want to throw an error)
// // Assumption: Network size is less than 32 nodes.
// bool first_val, second_val;
// for(size_t part = 0; part<longest_partition; ++part)
// {
// for(size_t b=31; b>=1; --b)
// {
// first_val = first[part+1] & (1 << b);
// second_val = second[part+1] & (1 << b);
//
// if(first_val > second_val) return 0;
// }
// }
//
// // There are 0 or more partitions to enumerate, so let's do it
//// gotoPartition(first);
// size_t distance = 0;
//
// bool reached_last = true;
// size_t longest_partition = (std::max)(partition[0], last[0]);
// for(size_t pos=0; pos<longest_partition; ++pos)
// reached_last &= (partition[pos] == last[pos]);
// 
// while(!reached_last)
// {
// nextPartition();
// distance++;
//
// reached_last = true;
// longest_partition = (std::max)(partition[0], last[0]);
// for(size_t pos=0; pos<longest_partition; ++pos)
// reached_last &= (partition[pos] == last[pos]);
// }
//
// return distance;
//}


bool PartitionEnumerator::isValidPartition(const unsigned long* partition)
{
    size_t accum = 0;
    
    for(size_t part=0; part<partition[0]; part++)
    {
        accum += partition[part+1];
    }
    
    return (accum == (unsigned int)((1 << partition[0]) - 1));
}


// -----------------------------------------------------------------------------------------
// Helper functions
// -----------------------------------------------------------------------------------------
void remapNodes(unsigned long* P_dest, const unsigned long* P_source, const size_t* node_map, size_t map_size)
{
    for(size_t part=0; part<P_source[0]; ++part)
        for(size_t node=0; node<map_size; ++node)
            if(P_source[part+1] & (1 << node)) 
                P_dest[part+1] |= (1 << node_map[node]);
}


P_uint64 Bell(size_t network_size)
{
    P_uint64 partition_count = 0;
    
    if(network_size)
    {
        std::vector<P_uint64> sizes(network_size * (network_size +1) / 2, 0);
        const  P_uint64 available = P_uint64(-1);
        
        sizes[0] = 1;
        
        size_t offset = 1;
        for(size_t r=2; r<=network_size; ++r)
        {
            sizes[offset] = sizes[offset - 1];
            
            for(size_t c=1; c<r; ++c)
            {
                // If overflow detected
                if((available - sizes[offset + c - 1]) <  sizes[offset + c - r])
                { 
                    std::cerr << "Error in bell(): Number of partitions doesn't fit in a 64-bits int. Returning zero." << std::endl;
                    return 0;
                }
                else
                {
                    sizes[offset + c] = sizes[offset + c - 1] + sizes[offset + c - r];
                }
            }
            offset += r;
        }
        
        partition_count = sizes.back();
    }
    
    return partition_count;
}


