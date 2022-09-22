public class Hello {
  public static void main(String[] args) {
    System.out.println("Hello, world!");
    var version = System.getProperty("java.version");
    System.out.printf("java.version = '%s'", version);
  }
}