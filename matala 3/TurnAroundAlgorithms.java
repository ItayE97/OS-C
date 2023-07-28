import java.util.ArrayList;
import java.util.LinkedList;
import java.util.PriorityQueue;

public class TurnAroundAlgorithms {
    final PriorityQueue<Process> processQueue; //Min-Heap based on arrival time
    TurnAroundAlgorithms(ArrayList<Process> p) //Ctor
    {
        ArrayList<Process> processList = p;
        processList.sort((p1,p2)->(int)(p1.arrivalTime-p2.arrivalTime));
        processQueue = new PriorityQueue<>((p1,p2)->(int)(p1.arrivalTime-p2.arrivalTime));
        processQueue.addAll(processList); 
    }

    Double FCFS()
    {
        Double sumOfTurnOvers = (double)0;
        Double currentTime = (double)0;
        for (Process p : processQueue) //Sorted by arrival order
        {
            currentTime = Math.max(currentTime, p.arrivalTime);
            currentTime += p.remainingWorkTime;
            p.remainingWorkTime =(double)0;
            p.finishTime = currentTime;
            sumOfTurnOvers += p.finishTime - p.arrivalTime;
        }
        resetProcessTimeCalculations();
        return sumOfTurnOvers/processQueue.size();
    }
    Double LCFS_NP()
    {
        Double sumOfTurnOvers = (double)0;
        Double currentTime = (double)0;
        PriorityQueue<Process> tmpQueue = new PriorityQueue<>((p1,p2)->(int)(p1.arrivalTime-p2.arrivalTime)); //Sorted by arrival time
        tmpQueue.addAll(processQueue);
        PriorityQueue<Process> reverseQueue = new PriorityQueue<>((p1,p2)->(int)(p2.arrivalTime-p1.arrivalTime)); //Sorted by LCFS
        ArrayList<Process> toBeRemoved = new ArrayList<>();

        reverseQueue.add(tmpQueue.poll()); //First process to arrive enters
        while(!reverseQueue.isEmpty())
        {
            Process p = reverseQueue.poll();
            currentTime = Math.max(currentTime, p.arrivalTime);
            currentTime += p.remainingWorkTime;
            p.remainingWorkTime = (double)0;
            p.finishTime = currentTime;
            sumOfTurnOvers += p.finishTime-p.arrivalTime;
            if(!tmpQueue.isEmpty()) //If other processes exist
            {
                for (Process process : tmpQueue) //Adds all the processes that had arrived and removes them from the normal queue
                {
                    if(process.arrivalTime <= currentTime) //Process has arrived
                    {
                        reverseQueue.add(process);
                        toBeRemoved.add(process);
                    }
                }

                if(toBeRemoved.isEmpty()) //Adds the next process that will arrive
                    reverseQueue.add(tmpQueue.poll());
                else
                {
                    tmpQueue.removeAll(toBeRemoved);
                    toBeRemoved.clear();    
                }    
            }
        }
        resetProcessTimeCalculations();
        return sumOfTurnOvers/processQueue.size();
    }
    Double LCFS_P()
    {
        Double sumOfTurnOvers = (double)0;
        Double currentTime = (double)0;
        PriorityQueue<Process> tmpQueue = new PriorityQueue<>((p1,p2)->(int)(p1.arrivalTime-p2.arrivalTime));
        tmpQueue.addAll(processQueue);
        PriorityQueue<Process> reverseQueue = new PriorityQueue<>((p1,p2)->(int)(p2.arrivalTime-p1.arrivalTime));
        ArrayList<Process> toBeRemoved = new ArrayList<>();

        reverseQueue.add(tmpQueue.poll()); //First process to arrive enters
        while(!reverseQueue.isEmpty())
        {
            Process p = reverseQueue.poll();
            currentTime = Math.max(currentTime, p.arrivalTime);
            if(!tmpQueue.isEmpty()) //Gets the time of the next process to enter the cpu
            {
                if(currentTime + p.remainingWorkTime < tmpQueue.peek().arrivalTime) //No process will arrive during workTime
                {
                    currentTime += p.remainingWorkTime;
                    p.remainingWorkTime = (double)0;
                    p.finishTime = currentTime;
                    currentTime = tmpQueue.peek().arrivalTime;
                }
                else //One or more will arrive
                {
                    double burstTime = tmpQueue.peek().arrivalTime - currentTime;
                    if(burstTime == 0) //Reorginize reverseQueue when multiplue processes arrive at the same time
                    {
                        for (Process process : reverseQueue)
                            toBeRemoved.add(process);
                        reverseQueue.clear();
                        reverseQueue.add(tmpQueue.poll());
                        reverseQueue.add(p);
                        reverseQueue.addAll(toBeRemoved);
                        toBeRemoved.clear();
                        continue;
                    }
                    else
                    {
                        currentTime += burstTime;
                        p.remainingWorkTime -= burstTime;
                        if(p.remainingWorkTime <= 0)
                        {
                            p.finishTime = currentTime - Math.abs(p.remainingWorkTime);
                            p.remainingWorkTime = (double)0; 
                        }  
                    }
                }
            }
            else //No more processes left
            {
                currentTime += p.remainingWorkTime;
                p.finishTime = currentTime;
                p.remainingWorkTime = (double)0;
            }

            if(p.remainingWorkTime <= 0) //Process p is finished
            {
                sumOfTurnOvers += p.finishTime - p.arrivalTime;
            }
            else //p isn't finished, adds it back to the queue
                reverseQueue.add(p);
            
            if(!tmpQueue.isEmpty())
            {    
                for (Process process : tmpQueue) //Adds all the processes that had arrived and removes them from the normal queue
                {
                    if(process.arrivalTime <= currentTime)
                    {
                        reverseQueue.add(process);
                        toBeRemoved.add(process);
                    }
                }

                if(toBeRemoved.isEmpty())
                    reverseQueue.add(tmpQueue.poll());
                else
                {
                    tmpQueue.removeAll(toBeRemoved);
                    toBeRemoved.clear();
                }    
            }
        }
        resetProcessTimeCalculations();
        return sumOfTurnOvers/processQueue.size();
    }
    Double RR(final int q)
    {
        Double sumOfTurnOvers = (double)0;
        Double currentTime = (double)0;
        int idleCounter = 0;
        LinkedList<Process> workingProcesses = new LinkedList<>();
        workingProcesses.addAll(processQueue); //All processes are added to a list ordered by arrival time
        
        currentTime = Math.max(currentTime, workingProcesses.peek().arrivalTime); //Set time to the arrival time of the first process
        for (int i = 0; workingProcesses.size() > 0; i++) 
        {
            if(workingProcesses.get(i).arrivalTime <= currentTime) //A process has arrived
            {
                Process p = workingProcesses.get(i);
                if(workingProcesses.size() > 1) //Other processes are left
                {
                    if(p.remainingWorkTime > q) //Process running for 'q' time
                    {
                        currentTime += q;
                        p.remainingWorkTime -= q;
                    }
                    else //Process done during 'q'
                    {
                        currentTime += p.remainingWorkTime;
                        p.finishTime = currentTime;
                        p.remainingWorkTime = (double)0;
                    }
                }
                else //No more processes left
                {
                    currentTime += p.remainingWorkTime;
                    p.finishTime = currentTime;
                    p.remainingWorkTime = (double)0;
                }

                if(p.remainingWorkTime <= 0) //Process p is finished
                {
                    sumOfTurnOvers += p.finishTime - p.arrivalTime;    
                    workingProcesses.remove(p);
                    i--;
                }
            }
            else //Process has not arrived
                idleCounter++;
            
            if(idleCounter == workingProcesses.size() && workingProcesses.size() > 0) //Jump to the next process incase cpu is idle
                currentTime = Math.max(currentTime,workingProcesses.get(0).arrivalTime); 

            if(i >= workingProcesses.size() -1) //resets the loop
            {
                i = -1; //i++ after this
                idleCounter = 0;
            }
        }
        resetProcessTimeCalculations();
        return sumOfTurnOvers/processQueue.size();
    }    
    Double SRTF()
    {
        Double sumOfTurnOvers = (double)0;
        Double currentTime = (double)0;
        ArrayList<Process> toBeRemoved = new ArrayList<>();
        LinkedList<Process> tmpQueue = new LinkedList<>();
        tmpQueue.addAll(processQueue); //All processes are added to a list ordered by arrival time
        PriorityQueue<Process> SJQueue = new PriorityQueue<>((p1,p2)->(int)(p1.totalWorkTime - p2.totalWorkTime)); //Min-Heap based on workTime

        SJQueue.add(tmpQueue.poll());
        currentTime = Math.max(currentTime, SJQueue.peek().arrivalTime); //Set time to the arrival time of the first process
        while (!SJQueue.isEmpty())
        {
           Process p = SJQueue.poll(); 
           currentTime = Math.max(currentTime,p.arrivalTime);
           if(!tmpQueue.isEmpty())
           {
               if(currentTime + p.remainingWorkTime <= tmpQueue.peek().arrivalTime) //No processes enter during 'p' workTime
               {
                   currentTime += p.remainingWorkTime;
                   p.finishTime = currentTime;
                   p.remainingWorkTime = (double)0;
               }
               else //One or more processes had entered during 'p' workTime
               {
                    double burstTime = tmpQueue.peek().arrivalTime - currentTime;
                    currentTime += burstTime;
                    p.remainingWorkTime -= burstTime;   
                    if(p.remainingWorkTime <= 0)
                    {
                        p.finishTime = currentTime - Math.abs(p.remainingWorkTime);
                        p.remainingWorkTime =(double)0;
                    }
               }
           }
           else //One or more processes had arrived
           {
                if(currentTime < p.arrivalTime)
                    currentTime = p.arrivalTime;
                currentTime += p.remainingWorkTime;
                p.remainingWorkTime = (double)0;
                p.finishTime = currentTime;
           }

           if(p.remainingWorkTime <= 0) //Process p is finished
           {
               sumOfTurnOvers += p.finishTime - p.arrivalTime;
           }
           else
                SJQueue.add(p);

           if(!tmpQueue.isEmpty())
           {
               for (Process process : tmpQueue) //Adds all the processes that had arrived and removes them from the normal queue
                {
                    if(process.arrivalTime <= currentTime)
                    {
                        SJQueue.add(process);
                        toBeRemoved.add(process);
                    }
                }

                if(toBeRemoved.isEmpty() && SJQueue.isEmpty())
                    SJQueue.add(tmpQueue.poll());
                else
                {
                    tmpQueue.removeAll(toBeRemoved);
                    toBeRemoved.clear();    
                }
           }
        }

        resetProcessTimeCalculations();
        return sumOfTurnOvers/processQueue.size();
    }

    void resetProcessTimeCalculations() //Sets remainingTime back to totalTime
    {
        for (Process process : processQueue)
        {
            process.remainingWorkTime = process.totalWorkTime;
            process.finishTime = (double)0;
        }
    }
}