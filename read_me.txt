1) LRU POLICY: Since lru has been declared as an int it has 32 bits , hence whenever valur of lru reaches INT_MAX the lru of the entire set is changed to zero
2) WRITEBACK: whenever a block with dirty bit 1 is being evicted , it will be counted as a writeback
3) the default has been set to direct , changes can be made for any kind of cache desired

