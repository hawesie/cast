package xsrad.logging;

import org.apache.log4j.Logger;



public class Port {
	
	private String url;
	private String context = "NONE";
	private String type;
	
	Port(String url, String type){
		this.url = url;
		this.type = type;
	}
	
	public void setContext(String context)
	{
		this.context = context;
	}
	
	String getContext() {
		return context;
	}
	
	String getType(){
		return type;
	}
	
	
	/*@Override
	public String toString()
	{
		String ret ="";
		ret += "<context>" + context + "</context>";
		ret += "<type>" + type + "</type>";
		return ret;
	}
	*/
	void log(String log)
	{
		Logger logger = Logger.getLogger("xsrad."+url);
		logger.trace("<content>" + log + "</content>");
	}
	
	public String keyValue(String key, String value) {
		return "" + key + "=\""+ value +"\" "; 
	}
	

}
