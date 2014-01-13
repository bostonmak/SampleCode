
package net.managers;

import net.Manager;
import state.ManagerType;
import state.PartConfig;
import state.KitConfig;
import javax.swing.*;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;
import java.util.*;
import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

@SuppressWarnings("serial")
public class KitManager extends Manager implements ActionListener{
	
	CardLayout cl;
	GridBagLayout gbl;
	GridBagConstraints c;
	JPanel createPanel, existingPanel, editPanel, existingButtonPanel;
	JLabel createKit, createKitName, createKitContents, createChoosePart, existingKit, existingKitName, 
		editKit, editKitName, editKitChoice, editKitContents, editChoosePart;
	JButton createSaveButton, createPartImageButton, existingRemoveButton, existingEditButton, existingBackButton, editSaveButton, editPartImageButton, 
		editBackButton, seeExistingButton, newConfigButton;
	JComboBox createPartsBox, editPartsBox;
	JScrollPane existingKitsPane;
	JTextField createKitText;
	JList existingKitsList;
	ArrayList<JButton> createContentButtons, existingContentButtons;
	ArrayList<String> partNameList, existingKitNameList, createContentsList;
	List<PartConfig> partConfigs;
	List<KitConfig> kitConfigs;
	String[] partNames, partImages, kitNames;
	ImageIcon partImage = new ImageIcon("gfx/blank.png");
	Map<String, Integer> map = new TreeMap<String, Integer>();
	String key;
	int kitIndex;
	

	public KitManager() {			// INSTANTIATE EVERYTHINGGGGGG
		super(ManagerType.Kit);
		// TODO Auto-generated constructor stub\
		
		cl = new CardLayout();
		setLayout(cl);
		gbl = new GridBagLayout();
		c = new GridBagConstraints();
		
		setSize(600, 600);
		setVisible(true);
		
		createPanel = new JPanel(gbl);
		existingPanel = new JPanel(gbl);
		editPanel = new JPanel(gbl);
		createPanel.setSize(600, 600);
		existingPanel.setSize(600, 600);
		editPanel.setSize(600, 600);
		existingButtonPanel = new JPanel(gbl);
		
		createKit = new JLabel("Create Kit Configuration");
		createKitName = new JLabel("Name of Kit: ");
		createKitContents = new JLabel("Kit Contents");
		createChoosePart = new JLabel("Current Part:");
		existingKit = new JLabel("Existing Kits");
		existingKitName = new JLabel("Names of Kits:");
		editKit = new JLabel("Edit Existing Kit");
		editKitName = new JLabel("Current Kit: ");
		editKitChoice = new JLabel("None");
		editKitContents = new JLabel("Kit Contents: ");
		editChoosePart = new JLabel("Current Part: ");
		
		createSaveButton = new JButton("Save");
		createPartImageButton = new JButton(partImage);
		existingRemoveButton = new JButton("Remove Kit");
		existingEditButton = new JButton("Edit Kit");
		existingBackButton = new JButton("Back");
		editSaveButton = new JButton("Save");
		editPartImageButton = new JButton(partImage);
		editBackButton = new JButton("Back");
		seeExistingButton = new JButton("See Existing Configs");
		newConfigButton = new JButton("Create New Config");
		
		createSaveButton.addActionListener(this);			// ADD ALL THE ACTIONS
		existingRemoveButton.addActionListener(this);
		existingEditButton.addActionListener(this);
		existingBackButton.addActionListener(this);
		editSaveButton.addActionListener(this);
		editBackButton.addActionListener(this);
		seeExistingButton.addActionListener(this);
		newConfigButton.addActionListener(this);
		
		createKitText = new JTextField(10);		
		
		createContentButtons = new ArrayList<JButton>();
		existingContentButtons = new ArrayList<JButton>();
		partNameList = new ArrayList<String>();
		existingKitNameList = new ArrayList<String>();
		createContentsList = new ArrayList<String>();
		partConfigs = Collections.synchronizedList(new ArrayList<PartConfig>());
		kitConfigs = new CopyOnWriteArrayList<KitConfig>();
		
		loadConfigData();									// LOAD ALL THE KITS AND PARTSSSSSS
		this.outMap.put("KitConfig", kitConfigs);
		partConfigs = (List<PartConfig>)inMap.get((String)"PartConfig");
		
		if (partConfigs != null)
		{
			partNames = new String[partConfigs.size() + 1];
			partImages = new String[partConfigs.size() + 1];
			for (int x = 0; x < partConfigs.size(); x ++)
			{
				partNames[x+1] = partConfigs.get(x).getPartName();
				partImages[x+1] = partConfigs.get(x).getPartName();
			}
		}
		else
		{
			partNames = new String[1];
			partImages = new String[1];
		}
		kitNames = new String[kitConfigs.size()];
		
		
		for (int x = 0; x < kitConfigs.size(); x++)
		{
			kitNames[x] = kitConfigs.get(x).kitName;
		}
		
		existingKitsList = new JList(kitNames);
		existingKitsPane = new JScrollPane(existingKitsList);
		
		c.insets = new Insets(10, 10, 10, 10);		//CONFIGURE DAT CREATE PANEL
		c.gridx = 3; c.gridy = 0;
		createPanel.add(createKit, c);
		c.gridx = 2; c.gridy = 2;
		createPanel.add(createKitName, c);
		c.gridx = 3; c.gridy = 2;
		createPanel.add(createKitText, c);
		c.gridx = 3; c.gridy = 3;
		createPanel.add(createKitContents, c);
		for (int y = 0; y < 3; y++)
		{
			for (int x = 0; x < 3; x++)
			{
				JButton b = new JButton(new ImageIcon("gfx/blank.png"));
				b.addActionListener(this);
				c.gridx = x + 2; c.gridy = y + 4;
				c.ipadx = 10; c.ipady = 10;
				c.fill = c.NONE;
				if (c.gridx == 2)
					c.anchor = c.LINE_END;
				else if (c.gridx == 3)
					c.anchor = c.CENTER;
				else
					c.anchor = c.LINE_START;
				if (createContentButtons.size() < 8)
				{
					createPanel.add(b, c);
					createContentButtons.add(b);
				}
			}
		}
		c.ipadx = 0; c.ipady = 0;		
		c.gridx = 2; c.gridy = 7;
		createPanel.add(createChoosePart, c);
		c.fill = c.HORIZONTAL;
		c.gridx = 3; c.gridy = 7;
		partNames[0] = "None";
		createPartsBox = new JComboBox(partNames);
		editPartsBox = new JComboBox(partNames);
		createPartsBox.addActionListener(this);
		editPartsBox.addActionListener(this);
		createPanel.add(createPartsBox, c);
		c.gridx = 4; c.gridy = 7;
		createPanel.add(createPartImageButton, c);
		c.gridx = 4; c.gridy = 8;
		createPanel.add(createSaveButton, c);
		c.gridx = 3; c.gridy = 8;
		c.fill = c.NONE;
		c.anchor = c.LINE_END;
		createPanel.add(seeExistingButton, c);
		
		c.gridx = 2; c.gridy = 0;		// DO DAT EXISTING PANEL
		c.fill = c.NONE;
		c.anchor = c.CENTER;
		existingPanel.add(existingKit, c);
		c.gridx = 2; c.gridy = 1;
		c.anchor = c.LINE_START;
		existingPanel.add(existingKitName, c);
		c.gridx = 2; c.gridy = 2;
		c.ipadx = 200; c.ipady = 50;
		existingPanel.add(existingKitsPane, c);
		c.ipadx = 0; c.ipady = 0;
		c.gridx = 2; c.gridy = 0;
		existingButtonPanel.add(existingRemoveButton, c);
		c.anchor = c.LINE_END;
		c.gridx = 4; c.gridy = 0;
		existingButtonPanel.add(existingEditButton, c);
		c.gridx = 0; c.gridy = 0;
		existingButtonPanel.add(existingBackButton, c);
		c.gridx = 2; c.gridy = 3;
		existingPanel.add(existingButtonPanel, c);

		
		c = new GridBagConstraints();				// COPY AND PASTE CREATE PANEL INTO EDIT PANEL YEAHHHH
		c.insets = new Insets(10, 10, 10, 10);
		c.gridx = 3; c.gridy = 0;
		editPanel.add(editKit, c);
		c.gridx = 2; c.gridy = 2;
		editPanel.add(editKitName, c);
		c.gridx = 3; c.gridy = 2;
		editPanel.add(editKitChoice, c);
		c.gridx = 3; c.gridy = 3;
		editPanel.add(editKitContents, c);
		for (int y = 0; y < 3; y++)
		{
			for (int x = 0; x < 3; x++)
			{
				JButton b = new JButton(new ImageIcon("gfx/blank.png"));
				b.addActionListener(this);
				c.gridx = x + 2; c.gridy = y + 4;
				c.ipadx = 10; c.ipady = 10;
				c.fill = c.NONE;
				if (c.gridx == 2)
					c.anchor = c.LINE_END;
				else if (c.gridx == 3)
					c.anchor = c.CENTER;
				else
					c.anchor = c.LINE_START;
				if (existingContentButtons.size() < 8)
				{
					editPanel.add(b, c);
					existingContentButtons.add(b);
				}
			}
		}
		c.ipadx = 0; c.ipady = 0;
		c.gridx = 2; c.gridy = 7;
		editPanel.add(editChoosePart, c);
		c.fill = c.HORIZONTAL;
		c.gridx = 3; c.gridy = 7;
		editPanel.add(editPartsBox, c);
		c.gridx = 4; c.gridy = 7;
		editPanel.add(editPartImageButton, c);
		c.gridx = 4; c.gridy = 8;
		editPanel.add(editSaveButton, c);
		c.gridx = 3; c.gridy = 8;
		c.fill = c.NONE;
		c.anchor = c.LINE_END;
		editPanel.add(newConfigButton, c);
		c.gridx = 2; c.gridy = 8;
		editPanel.add(editBackButton, c);
		
		
		add(createPanel, "Create");
		add(existingPanel, "Existing");
		add(editPanel, "Edit");
		
		cl.show(this.getContentPane(), "Create");
	}

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		
		KitManager km = new KitManager();
		//km.loadData("data/partConfigList.ser");

	}
	
	public void saveConfigData(String path)			//SAVE ALL THE KITS
	{
		try
		{
			FileOutputStream fout = new FileOutputStream(path);
			ObjectOutputStream oout = new ObjectOutputStream(fout);
			oout.writeObject(kitConfigs);
			fout.close();
			oout.close();
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	
	public void loadConfigData()		//LOAD ALL THE KITS
	{
		try {
			FileInputStream fileIn = new FileInputStream("data/kitConfigList.ser");
			ObjectInputStream in = new ObjectInputStream(fileIn);
			kitConfigs = (List<KitConfig>) in.readObject();
			in.close();
			fileIn.close();
			kitNames = new String[kitConfigs.size()];
			for (int x = 0; x < kitConfigs.size(); x++)
			{
				kitNames[x] = kitConfigs.get(x).kitName;
			}
		} catch (IOException i) {
			i.printStackTrace();
		} catch (ClassNotFoundException c) {
			System.out.println("Failed to find kit configs");
			c.printStackTrace();
		}
	}
	
	public void loadPartData(String path) {		//LOAD ALL THE PARTS
		try {
			FileInputStream fileIn = new FileInputStream(path);
			ObjectInputStream in = new ObjectInputStream(fileIn);
			partConfigs = (CopyOnWriteArrayList<PartConfig>) in.readObject();
			in.close();
			fileIn.close();
			partNames = new String[partConfigs.size() + 1];
			partImages = new String[partConfigs.size() + 1];
			partNames[0] = "None";
			partImages[0] = "blank.png";
			for (int x = 1; x < partConfigs.size(); x++)
			{
				partNames[x] = partConfigs.get(x).getPartName();
				partImages[x] = partConfigs.get(x).getImageFile();
			}
		} catch (IOException i) {
			i.printStackTrace();
		} catch (ClassNotFoundException c) {
			System.out.println("GUI Part class not found");
			c.printStackTrace();
		}
	}
	
	public void actionPerformed(ActionEvent ae)			//PERFORM ALL THE ACTIONS
	{
		for (int x = 0; x < createContentButtons.size(); x++)		//CONFIG BUTTON ACTIONS
		{
			c = new GridBagConstraints();
			if (ae.getSource() == createContentButtons.get(x))
			{
				createContentButtons.get(x).setIcon(createPartImageButton.getIcon());
			}
			else if (ae.getSource() == existingContentButtons.get(x))
			{
				existingContentButtons.get(x).setIcon(editPartImageButton.getIcon());
			}
		}
		if (ae.getSource() == createSaveButton)			//SAAAAAVE
		{
			int count = 0;
			existingPanel.remove(existingKitsPane);
			String s = createKitText.getText();
			if (s.equals(""))
			{
				s = "null";
			}
			Map<String, Integer> m = new TreeMap<String, Integer>();
			for (int x = 0; x < createContentButtons.size(); x++)
			{
				String key = createContentButtons.get(x).getIcon().toString().substring(4, createContentButtons.get(x).getIcon().toString().length() - 4);
				if (!key.equals("blank"))
				{
					if (m.containsKey(key))
					{
						m.put(key, m.get(key) + 1);
					}
					else
					{
						m.put(key, 1);
					}
					count++;
				}
			}
			boolean found = false;
			if (count >= 4)
			{
				for (int x = 0; x < kitConfigs.size(); x++)
				{
					if (kitConfigs.get(x).kitName.equals(s))
					{
						System.out.println("Saved " + kitConfigs.get(x).kitName);
						kitConfigs.get(x).components = m;
						found = true;
					}
				}
				if (!found)
				{
					KitConfig k = new KitConfig(s, m);
					kitConfigs.add(k);
					
				}
				System.out.println("Saved config");
				
				kitNames = new String[kitConfigs.size()];
				for (int x = 0; x < kitConfigs.size(); x++)
				{
					kitNames[x] = kitConfigs.get(x).kitName;
				}
				
				saveConfigData("data/kitConfigList.ser");
			}
			else
			{
				JOptionPane.showMessageDialog(null,
						"You need at least 4 parts in a kit",
						"Error",
						JOptionPane.ERROR_MESSAGE);
			}
			existingKitsList = new JList(kitNames);
			existingKitsPane = new JScrollPane(existingKitsList);
			
			c.gridx = 2; c.gridy = 2;
			c.ipadx = 200; c.ipady = 50;
			existingPanel.add(existingKitsPane, c);
			
		}
		else if (ae.getSource() == existingRemoveButton)		//REMOOOOOOOOOOOOOVE
		{
			//remove a config somehow
			if (existingKitsList.getSelectedIndex() > -1)
			{
				kitConfigs.remove(existingKitsList.getSelectedIndex());
				existingPanel.remove(existingKitsPane);
				kitNames = new String[kitConfigs.size()];
				for (int x = 0; x < kitConfigs.size(); x++)
				{
					kitNames[x] = kitConfigs.get(x).kitName;
				}
				existingKitsList = new JList(kitNames);
				existingKitsPane = new JScrollPane(existingKitsList);
				this.saveConfigData("data/kitConfigList.ser");
				c.gridx = 2; c.gridy = 2;
				c.ipadx = 200; c.ipady = 50;
				existingPanel.add(existingKitsPane, c);
				existingPanel.repaint();
				this.repaint();
			}
			System.out.println("Removed config");
			System.out.println("Kits in list: " + kitConfigs.size());
			this.repaint();
			cl.show(this.getContentPane(), "Create");
			cl.show(this.getContentPane(), "Existing");
			
		}
		else if (ae.getSource() == existingEditButton)		//EDIT THE KIIIIIIIT
		{
			
			if (existingKitsList.getSelectedIndex() > -1)
			{
				kitIndex = existingKitsList.getSelectedIndex();				
				String name = kitNames[kitIndex];
				editKitChoice.setText(name);
				if (editKitChoice.getText().equals(kitConfigs.get(kitIndex).kitName))
				{
					Map<Integer, String> m = new TreeMap<Integer, String>();
					int count = 0;
					for (int y = 0; y < this.partConfigs.size(); y++)
					{
						System.out.println(partConfigs.get(y).getImageFile().substring(0, partConfigs.get(y).getImageFile().length() - 4));
						if (kitConfigs.get(kitIndex).components.containsKey(partConfigs.get(y).getImageFile().substring(0, partConfigs.get(y).getImageFile().length() - 4)))
						{
							for (int z = 0; z < kitConfigs.get(kitIndex).components.get(partConfigs.get(y).getImageFile().substring(0, partConfigs.get(y).getImageFile().length() -4)); z++)
							{
								m.put(count, partConfigs.get(y).getImageFile());
								count++;
							}
						}
					}
					if (count > 8)
					{
						System.out.println("BIG ERROR");
					}
					else if (count < 8)
					{
						while(count < 8)
						{
							m.put(count, "blank.png");
							count++;
							System.out.println("added blank");
						}
					}
					
					for (int y = 0; y < existingContentButtons.size(); y++)
					{
						existingContentButtons.get(y).setIcon(new ImageIcon(new String("gfx/" + m.get(y))));
					}
				}
			
				
			}
			else
				editKitChoice.setText("None");
			cl.show(this.getContentPane(), "Edit");
			System.out.println("Moving to edit");
		}
		else if (ae.getSource() == editSaveButton)		//SAAAAAAAAAAAAAVE THE EXISTING
		{
			int count = 0;
			existingPanel.remove(existingKitsPane);
			String s = createKitText.getText();
			if (s.equals(""))
			{
				s = "null";
			}
			Map<String, Integer> m = new TreeMap<String, Integer>();
			for (int x = 0; x < existingContentButtons.size(); x++)
			{
				String key = existingContentButtons.get(x).getIcon().toString().substring(4, existingContentButtons.get(x).getIcon().toString().length() - 4);
				if (!key.equals("blank"))
				{
					if (m.containsKey(key))
					{
						m.put(key, m.get(key) + 1);
						
					}
					else
					{
						m.put(key, 1);
					}
					count++;
				}
			}
			if (count >= 4)
			{
				kitConfigs.get(kitIndex).components = m;
				System.out.println("Saved config");
				saveConfigData("data/kitConfigList.ser");
			}
			else
			{
				JOptionPane.showMessageDialog(null,
						"You need at least 4 parts in a kit",
						"Error",
						JOptionPane.ERROR_MESSAGE);
			}
			System.out.println("Kit Size = " + count); 
		}
		else if (ae.getSource() == seeExistingButton || ae.getSource() == editBackButton)		//LETS GO TO THE EXISTING PANEL!
		{
			cl.show(this.getContentPane(), "Existing");
			System.out.println("Moving to existing");
		}
		else if (ae.getSource() == newConfigButton || ae.getSource() == existingBackButton)		//LETS CREATE SOME KIT!
		{
			cl.show(this.getContentPane(), "Create");
			System.out.println("Moving to create");
		}
		else if (ae.getSource() == createPartsBox)		//SELECT ANY PART YOU LIEK?
		{
			if (partConfigs.size() > 0)
			{
				if (createPartsBox.getSelectedIndex() - 1 > -1)
				{
					partImage = new ImageIcon(new String("gfx/" + partConfigs.get((createPartsBox.getSelectedIndex()) - 1).getImageFile()));
					createPartImageButton.setIcon(partImage);
				}
				else 
				{
					partImage = new ImageIcon("gfx/blank.png");
					createPartImageButton.setIcon(partImage);
				}
			}
		}
		else if (ae.getSource() == editPartsBox)		// I HAS PARTS. YOU LIEK?
		{
			if (partConfigs.size() > 0)
			{
				if (editPartsBox.getSelectedIndex() - 1 > -1)
				{
					partImage = new ImageIcon(new String("gfx/" + partConfigs.get(editPartsBox.getSelectedIndex() - 1).getImageFile()));
					editPartImageButton.setIcon(partImage);
				}
				else
				{
					partImage = new ImageIcon("gfx/blank.png");
					editPartImageButton.setIcon(partImage);
				}
			}
		}
		
		
		
		this.outMap.put("KitConfig", kitConfigs);
		partConfigs = (List<PartConfig>)inMap.get((String)"PartConfig");
		this.repaint();
	}

}
