import java.util.*;
import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;

public class MainClass {
    public static void runTurnaroundTests(FileInputStream input) throws IOException
    {
        BufferedReader bf = new BufferedReader(new InputStreamReader(input));
        int numOfProcesses = Integer.parseInt(bf.readLine());
        ArrayList<Process> p = new ArrayList<>();
        for (int i = 0; i < numOfProcesses; i++) {
            String line = bf.readLine();
            String[] tmp = line.split(",");
            p.add(new Process("P"+(i+1), Double.parseDouble(tmp[0]), Double.parseDouble(tmp[1])));
        }
        TurnAroundAlgorithms algs = new TurnAroundAlgorithms(p);
        System.out.println("FCFS: mean turnaround = "+algs.FCFS());
        System.out.println("LCFS(NP): mean turnaround = "+algs.LCFS_NP());
        System.out.println("LCFS(P): mean turnaround = "+algs.LCFS_P());
        System.out.println("RR: mean turnaround = "+algs.RR(2));
        System.out.println("SJF(STRF): mean turnaround = "+algs.SRTF());
    }
    public static void main(String[] args) throws IOException {
        FileInputStream input = null;
        int numOfFiles = 5;
        for (int i = 0; i < numOfFiles; i++) 
        {
            String fileName = "input"+(i+1)+".txt";
            try {
                input = new FileInputStream(fileName);
            } catch (FileNotFoundException e) {
                input.close();
                System.out.println(new FileNotFoundException("File '"+fileName+"' not found!"));
                continue;
            }
            System.out.println("Running tests on file '"+fileName+"'");
            runTurnaroundTests(input);
            input.close();
            System.out.println("End of file '"+fileName+"'");
            System.out.println();
        }
        System.out.println("Done!");
    }
}
