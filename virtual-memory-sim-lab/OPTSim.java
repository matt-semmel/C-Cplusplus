import java.io.File;
import java.io.FileNotFoundException;
import java.util.*;

public class OPTSim implements MemSim
{
    // Init the vars for ze simulation
    int pageSize;
    int numFrames;
    int memAccesses;
    int diskWrites;
    int pageFaults;

    OPTProcess[] processes = new OPTProcess[2];

    @SuppressWarnings("unchecked")
    public OPTSim(int nf, int ps, int p1, int p2, File f, int pno) throws FileNotFoundException
    {
        numFrames = nf;
        pageSize = ps;

        Scanner fileRead = new Scanner(f);
        HashMap<Long, PageTableEntry>[] processMemory = new HashMap[2];
        processMemory[0] = new HashMap<>();
        processMemory[1] = new HashMap<>();
        int line = 0;

        // Read the trace file and populate ze map of hash.
        while(fileRead.hasNextLine())
        {
            // Get next line of the file and parse in the address
            // Retrieving only the page number
            // Then get the current process accessing the address

            String[] data = fileRead.nextLine().split(" ");
            int process = Integer.parseInt(data[2]);
            HashMap<Long, PageTableEntry> temp = processMemory[process];
            long pageNum = Long.decode(data[1]);
            pageNum = pageNum >> pno;

            // Make a new entry if the pagenum doesn't exist as a key
            // Else add curr line to PTE's list.
            PageTableEntry page = temp.get(pageNum);
            
            if(page == null)
            {
                page = new PageTableEntry(line, pageNum);
                page.accesses.add(line);
                temp.put(pageNum, page);
            }

            else
            {
                page.accesses.add(line);
            }
            
            line++;
        }

        // Close the file when you're done! Derp.
        fileRead.close();

        // Calc number of frames allocated to each process and pass it to OPTProcess
        processes[0] = new OPTProcess(((nf/(p1+p2)) * p1), processMemory[0]);
        processes[1] = new OPTProcess(((nf/(p1+p2)) * p2), processMemory[1]);
        memAccesses = 0;
        pageFaults = 0;
        diskWrites = 0;
    }

    public void simulate(char accessType, long pageNum, int process, int line)
    {
        // Increment memory access count, get the current process, then reference the PTE with matching page num.
        memAccesses++;
        OPTProcess currProc = processes[process];
        PageTableEntry currPage = currProc.pageTable.get(pageNum);
        currPage.accesses.remove(0);

        // Check to see if dirty bit needs to be set.
        if(accessType == 's')
        {
            currPage.written = true;
        }

        //Set the last access of the page to the current line
        currPage.lastAccess = line;
        
        //If the current process is already in RAM, update entry.
        if(!currProc.ram.contains(pageNum))
        {
            pageFaults++;
            
            // If the page not in RAM, check space to add it
            // If not enough space select a page to evict and remove from RAM
            // Also check dirty bit to see if there is a disk write
            if(currProc.ram.size() == currProc.maxSize)
            {
                Long toRemove = getOptimalPage(currProc);
                currProc.ram.remove(toRemove);
                if(currProc.pageTable.get(toRemove).written)
                {
                    diskWrites++;
                    currProc.pageTable.get(toRemove).written = false;
                }

            }
            
            currProc.ram.add(currPage.pageNum);
        }

    }

    public Long getOptimalPage(OPTProcess o)
    {
        ArrayList<PageTableEntry> noAccesses = new ArrayList<>();
        int nextAccess = Integer.MIN_VALUE;
        Long bestPage = null;
        for(Long l : o.ram)
        {
            PageTableEntry value = o.pageTable.get(l);

            // If no future accesses, add the page to the bestPages array
            
            if(value.accesses.isEmpty())
            {
                noAccesses.add(value);
            }
            else
            {
                // If the current page has an access after the current best page to remove
                // set the new page to the best one to remove
                
                if(value.accesses.get(0) > nextAccess)
                {
                    bestPage = value.pageNum;
                    nextAccess = value.accesses.get(0);
                }

            }
        }
        // Check if there are pages with no future accesses
        // Get process with no future accesses that were LRU
        if(!noAccesses.isEmpty())
        {
            int lastAccess = Integer.MAX_VALUE;
            for (PageTableEntry noAccessPage : noAccesses)
            {
                if (noAccessPage.lastAccess < lastAccess)
                {
                    bestPage = noAccessPage.pageNum;
                    lastAccess = noAccessPage.lastAccess;
                }
            }
        }
        return bestPage;
    }

    public String toString()
    {
        return "Algorithm: OPT\n" +
                "Number of frames: " + numFrames + "\n" +
                "Page size: " + pageSize + " KB\n" +
                "Total memory accesses: " + memAccesses + "\n" +
                "Total page faults: " + pageFaults + "\n" +
                "Total writes to disk: " + diskWrites;
    }

    // OPT process. Hashset representing current pages in RAM. Hashmap of line accesses per page
    private class OPTProcess
    {
        HashSet<Long> ram;
        HashMap<Long, PageTableEntry> pageTable;
        int maxSize;

        private OPTProcess(int f, HashMap<Long, PageTableEntry> h)
        {
            ram = new HashSet<>();
            pageTable = h;
            maxSize = f;
        }

    }

}
