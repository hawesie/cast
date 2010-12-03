package xsrad.logging;

public class PortWithActionCondition extends Port {

	private String action = "NONE";
	private String condition = "NONE";
	private String dataType = "NONE";
	
	PortWithActionCondition(String url, String type) {
		super(url,type);
	}
	
	public void setAction(String action)
	{
		this.action = action;
	}
	
	public void setCondition(String condition)
	{
		this.condition = condition;
	}
	
	public void setDataType(String dataType)
	{
		this.dataType = dataType;
	}
	

	
	/*@Override
	public String toString()
	{
		String ret = super.toString();
		ret += "<action>" + action + "</action>";
		ret += "<condition>" + condition + "</condition>";
		ret += "<datatype>" + dataType + "</datatype>";
		return ret;
	}*/
	
	public String toString()
	{
		String ret = "<" + getType() + " ";
		ret += keyValue("action", getAction());
		ret += keyValue("condition", getCondition());
		ret += keyValue("context", getContext());
		ret += keyValue("datatype", getDatatype());
		ret += "/>";
		return ret;
	}
	
	
	String getCondition(){
		return condition;
	}
	
	String getDatatype(){
		return dataType;
	}
	
	String getAction(){
		return action;
	}
	

	public void log()
	{
		super.log(toString());
	}
	
	
	
}
