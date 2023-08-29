import java.io.File;
import java.io.FileNotFoundException;
import java.util.*;

public class vmsim
{
    public static void main(String[] args) throws FileNotFoundException {
        // Initialize the command line args. Should be self-explanatory from var names.
        String algorithm = null;
        int numFrames = 0;
        int pageSize = 0;
        int pageNumberOffset = 0;
        int line = 0;

        // split memory
        String[] memSplit = null;
        int a = 0;
        int b = 0;

        // trace files
        String fileName = null;
        File traceFile = null;
        Scanner fileRead = null;

        // Check args length to make sure we aren't missing anything
        if(args.length != 9)
        {
            System.out.println("\nPlease check your arguments! Did you forget something?\nExample: java vmsim -a lru -n 256 -p 64 -s 1:2 4-hmmer-mcf.trace");
            System.exit(-1);
        }

        // Parse info from args
        try
        {
            algorithm = args[1];
            numFrames = Integer.parseInt(args[3]);
            pageSize = Integer.parseInt(args[5]);
            memSplit = args[7].split(":");
            fileName = args[8];
            a = Integer.parseInt(memSplit[0]);
            b = Integer.parseInt(memSplit[1]);
            
            // Calculate the number of hex digits used for the page offset in the trace file.
            
            pageNumberOffset = (int) Math.ceil((((Math.log(pageSize) / Math.log(2))) + 10));
        }
        
        catch(NumberFormatException nf)
        {
            // If args improperly formatted 
            System.out.println("\nPlease check your arguments! You may have a letter where there should be a number or something.\\nExample: java vmsim -a lru -n 256 -p 64 -s 1:2 4-hmmer-mcf.trace");
            System.exit(-1);
        }

        // Read the specified trace file or exit if the file doesn't exist or can't be read
        try
        {
            traceFile = new File(fileName);
            fileRead = new Scanner(traceFile);
        }
        
        catch (FileNotFoundException e)
        {
            System.out.println("\nYou dun goofed!\nThe file you specified does not exist or could not be read!\nConsequences will never be the same!");
            System.exit(-1);
        }

        MemSim simulation = null;

        // Select the specified algorithm simulation or exit the program if it's not "opt" or "lru"
        if(algorithm.equals("opt"))
        {
            simulation = new OPTSim(numFrames, pageSize, a, b, traceFile, pageNumberOffset);
        }

        else if(algorithm.equals("lru"))
        {
            simulation = new LRUSim(numFrames, pageSize, a, b);
        }

        else
        {
            System.out.println("\nPlease select either 'opt' or 'lru' as your algorithm (-a)!");
            System.exit(-1);
        }

        // Init vars for simulating the algo then loop through the given trace file.
        char accessType = 0;
        long pageNum = 0;
        int process = 0;
        
        while (fileRead.hasNextLine())
        {
            String[] data = fileRead.nextLine().split(" ");
            try
            {
                accessType = data[0].charAt(0);
                /*
                    Convert only the digits representing the page number to a decimal value to a long.
                    This is calculated by taking only the digits from 0 to the total # - the offset
                 */

                pageNum = Long.decode(data[1]);
                pageNum = pageNum >>> pageNumberOffset;
                process = Integer.parseInt(data[2]);
            }
            catch(NumberFormatException nf)
            {
                System.out.println("\nInvalid trace!\nSomething in the formatting is preventing the trace from being read!");
                nf.printStackTrace();
                System.exit(-1);
            }

            simulation.simulate(accessType, pageNum, process, line);
            line++;

        }

        System.out.println(simulation);
    
    }

}