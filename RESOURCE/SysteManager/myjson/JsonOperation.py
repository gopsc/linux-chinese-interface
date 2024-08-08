import json

class JsonOperation():

    def save_dict_as_json(dictionary, file_path):  
        """  
        将字典保存为JSON格式的文件。  
    
        Parameters:  
        dictionary (dict): 要保存的字典数据。  
        file_path (str): JSON文件的保存路径。  
        """  
        try:  
            with open(file_path, 'w', encoding='utf-8') as json_file:  
                json.dump(dictionary, json_file, ensure_ascii=False, indent=4)  
            print(f"字典已成功保存为JSON文件：{file_path}")  
        except IOError as e:  
            print(f"保存JSON文件时出错：{e}") 


    def json_file_to_dict(file_path):  
        """  
        读取JSON文件并将其转换为字典。  
    
        Parameters:  
        file_path (str): JSON文件的路径。  
    
        Returns:  
        dict: 解析后的字典数据。  
        """  
        try:  
            with open(file_path, 'r', encoding='utf-8') as json_file:  
                data_dict = json.load(json_file)  
            return data_dict  
        except FileNotFoundError:  
            print(f"文件未找到：{file_path}")  
            return None  
        except json.JSONDecodeError:  
            print(f"文件不是有效的JSON格式：{file_path}")  
            return None  
        except IOError as e:  
            print(f"读取文件时出错：{e}")  
            return None  