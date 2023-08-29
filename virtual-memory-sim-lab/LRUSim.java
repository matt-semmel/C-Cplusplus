import java.util.LinkedList;

public class LRUSim implements MemSim
{
    // Init vars for ze simulation!
    int pageSize;
    int numFrames;
    int memAccesses;
    int pageFaults;
    int diskWrites;

    LRUProcess[] processes = new LRUProcess[2];

    public LRUSim(int nf, int ps, int p1, int p2)
    {
        // Calc number of frames allocated to each process and initializing access data
        numFrames = nf;
        pageSize = ps;
        processes[0] = new LRUProcess((nf/(p1+p2)) * p1);
        processes[1] = new LRUProcess((nf/(p1+p2)) * p2);
        memAccesses = 0;
        pageFaults = 0;
        diskWrites = 0;
    }

    public void simulate(char accessType, long pageNum, int process, int line)
    {
        // Increment the number of memory accesses, check if the page is in RAM
        memAccesses++;
        LRUProcess currProc = processes[process];
        PageTableEntry currPage = getPage(currProc, pageNum);
        
        // If the page isn't in RAM, then we need to create and add it
        if(currPage == null)
        {
            pageFaults++;
            currPage = new PageTableEntry(pageNum);
            if (accessType == 's')
            {
                currPage.written = true;
            }
            
            // Add the process if enough space in RAM
            if(currProc.loaded < currProc.maxSize)
            {
                currProc.ram.add(currPage);
                currProc.loaded++;
            }
            else
            {
                // If RAM doesn't have enough space, evict the first entry and add the new entry
                PageTableEntry temp = currProc.ram.remove(0);
                if(temp.written == true)
                {
                    diskWrites++;
                }
                currProc.ram.add(currPage);
            }

        }
        else
        {
            if (accessType == 's')
            {
                currPage.written = true;
            }
        }

    }

    public PageTableEntry getPage(LRUProcess p, long pageNum)
    {
        // Check if there RAM empty, look for the page, then add process to list when referenced
        if(!p.ram.isEmpty())
        {
            for(int i = 0; i < p.ram.size(); i++)
            {
                PageTableEntry currPage = p.ram.get(i);
                if(currPage.pageNum == pageNum)
                {
                    p.ram.add(p.ram.remove(i));
                    return currPage;
                }
            }
        }
        return null;
    }

    public String toString()
    {
        return "Algorithm: LRU\n" +
                "Number of frames: " + numFrames + "\n" +
                "Page size: " + pageSize + " KB\n" +
                "Total memory accesses: " + memAccesses + "\n" +
                "Total page faults: " + pageFaults + "\n" +
                "Total writes to disk: " + diskWrites;
    }

    private class LRUProcess
    {
        long maxSize;
        LinkedList<PageTableEntry> ram;
        int loaded;

        // Set max size to frame size and init LL+vars
        private LRUProcess(int f)
        {
            maxSize = f;
            ram = new LinkedList<PageTableEntry>();
            loaded = 0;
        }

    }
}
