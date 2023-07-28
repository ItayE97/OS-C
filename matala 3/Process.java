public class Process{
    final String name;
    final Double arrivalTime;
    final Double totalWorkTime;
    Double remainingWorkTime;
    Double finishTime;
    
    Process(String name,Double start, Double work)
    {
       this.name = name;
       arrivalTime = start;
       totalWorkTime = work;
       remainingWorkTime = work;
       finishTime = (double)0; 
    }
}
