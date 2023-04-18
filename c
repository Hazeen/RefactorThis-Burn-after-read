 public static void DataSet2ModelDictionary(DataSet ds, Dictionary<int, Model> modelDic,
                                                    List<System.Windows.Forms.CheckBox> cbs,
                                                    List<System.Windows.Forms.GroupBox> gbs,
                                                    List<System.Windows.Forms.TextBox> tbs)
        {
            var modelsTable = ds.Tables["Models"];
            if (modelsTable != null)
            {
                // if the modelsTable has records, loop through all rows and update models
                foreach (DataRow row in modelsTable.Rows)
                {
                    var id = row.Field<int>("ID");
                    if (modelDic.ContainsKey(id - 1)) // if the model in the modelsTable is n the modelDic then update it
                    {
                        UpdateModelFromDS(ds, modelDic[id - 1], id, cbs, gbs, tbs);
                    }
                }
                // if the modelsTable has records, loop through all rows and add models not in the modelDic
                foreach (DataRow row in modelsTable.Rows)
                {
                    var id = row.Field<int>("ID");
                    if (!modelDic.ContainsKey(id - 1)) // if the model in the modelsTable is n the modelDic then update it
                    {
                        var m = GetNewModelFromDS(ds, id, cbs, gbs, tbs);
                        modelDic.Add(id - 1, m);
                    }
                }

                // if the modelDic has record that is not in the modelsTable then remove it from the dictionary
                List<int> keysToRemove = new List<int>();
                foreach (var key in modelDic.Keys)
                {
                    if (!modelsTable.AsEnumerable().Any(row => row.Field<int>("ID") == key + 1))
                    {
                        keysToRemove.Add(key);
                    }
                }
                foreach (var key in keysToRemove)
                {
                    modelDic.Remove(key);
                }
            }
            else // if the modelsTable is null make sure that the modelDic is new fresh one.
            {
                modelDic = new Dictionary<int, Model>();
            }
        }


private static Model GetNewModelFromDS(DataSet ds, int id,
                                                    List<System.Windows.Forms.CheckBox> cbs,
                                                    List<System.Windows.Forms.GroupBox> gbs,
                                                    List<System.Windows.Forms.TextBox> tbs)
        {
            var model = new Model();

            // Models table
            var modelsTable = ds.Tables["Models"];
            if (modelsTable != null)
            {
                var row = modelsTable.Rows[id - 1];
                model.ModelName = row.Field<string>("Model name");
                model.ModelDescription = row.Field<string>("Model description");
                // define some gloval variables
                var moveAx = GetGBValue(gbs.Find(x => x.Name == "MovingAxies_GB"));
                var slewAx = GetGBValue(gbs.Find(x => x.Name == "SlewingAxies_GB"));
                var luffAx = GetGBValue(gbs.Find(x => x.Name == "LuffingAxies_GB"));
                uint result = 0;

                var nodeTranslation = new NodeTranslation();
                if (cbs.Find(x => x.Name == "Moving_CB").CheckState == CheckState.Checked)
                {
                    nodeTranslation.TranslationID = 1;
                    nodeTranslation.TranslationText = "Moving";
                    nodeTranslation.NodeList = tbs.Find(x => x.Name == "MovingNodesList_TB").Text;
                    nodeTranslation.Distance = row.Field<double>("Translation");
                    nodeTranslation.FixedVectorA = new double[] { 0, 0, 0 };
                    nodeTranslation.FixedVectorB = moveAx;
                }
                model.NodeTranslationList.Add(nodeTranslation);

                var nodeRotation = new NodeRotation();
                if (cbs.Find(x => x.Name == "Slewing_CB").CheckState == CheckState.Checked)
                {
                    nodeRotation.RotationID = 1;
                    nodeRotation.RotationText = "Slew";
                    nodeRotation.IsParallelogram = false;
                    nodeRotation.NodeList = tbs.Find(x => x.Name == "SlewNodesList_TB").Text;
                    nodeRotation.RotationAngle = row.Field<double>("Slew");
                    nodeRotation.NodeA1 = UInt32.TryParse(tbs.Find(x => x.Name == "SlewNode_TB")?.Text, out result) ? result : 0;
                    nodeRotation.NodeA2 = nodeRotation.NodeA1;
                    nodeRotation.VectorA1 = new double[] { 0, 0, 0 };
                    nodeRotation.VectorA2 = slewAx;
                };

                model.NodeRotationList.Add(nodeRotation);
                if (cbs.Find(x => x.Name == "TwinRocker_CB").CheckState == CheckState.Checked &&
                    cbs.Find(x => x.Name == "Luffing_CB").CheckState == CheckState.Checked)
                {
                    nodeRotation = new NodeRotation();
                    nodeRotation.RotationID = 2;
                    nodeRotation.RotationText = "Luff";
                    nodeRotation.IsParallelogram = false;
                    nodeRotation.NodeList = tbs.Find(x => x.Name == "LuffNodesListFront_TB").Text;
                    nodeRotation.RotationAngle = row.Field<double>("Luff");
                    nodeRotation.NodeA1 = UInt32.TryParse(tbs.Find(x => x.Name == "LuffNodeA_TB")?.Text, out result) ? result : 0;
                    nodeRotation.NodeA2 = nodeRotation.NodeA1;
                    nodeRotation.VectorA1 = new double[] { 0, 0, 0 };
                    nodeRotation.VectorA2 = luffAx;

                    model.NodeRotationList.Add(nodeRotation);

                    nodeRotation = new NodeRotation();
                    nodeRotation.RotationID = 3;
                    nodeRotation.RotationText = "Luff";
                    nodeRotation.IsParallelogram = true;
                    nodeRotation.NodeList = tbs.Find(x => x.Name == "LuffNodesListRear_TB").Text;
                    nodeRotation.RotationAngle = row.Field<double>("Luff");
                    nodeRotation.NodeA1 = UInt32.TryParse(tbs.Find(x => x.Name == "LuffNodeA_TB")?.Text, out result) ? result : 0;
                    nodeRotation.NodeA2 = nodeRotation.NodeA1;
                    nodeRotation.VectorA1 = new double[] { 0, 0, 0 };
                    nodeRotation.VectorA2 = luffAx;
                    nodeRotation.NodeB1 = UInt32.TryParse(tbs.Find(x => x.Name == "LuffNodeB_TB")?.Text, out result) ? result : 0;
                    nodeRotation.NodeB2 = nodeRotation.NodeB1;
                    nodeRotation.VectorB1 = new double[] { 0, 0, 0 };
                    nodeRotation.VectorB2 = luffAx;
                    nodeRotation.NodeC1 = UInt32.TryParse(tbs.Find(x => x.Name == "LuffNodeC_TB")?.Text, out result) ? result : 0;
                    nodeRotation.NodeC2 = nodeRotation.NodeC1;
                    nodeRotation.VectorC1 = new double[] { 0, 0, 0 };
                    nodeRotation.VectorC2 = luffAx;
                    nodeRotation.NodeD1 = UInt32.TryParse(tbs.Find(x => x.Name == "LuffNodeD_TB")?.Text, out result) ? result : 0;
                    nodeRotation.NodeD2 = nodeRotation.NodeD1;
                    nodeRotation.VectorD1 = new double[] { 0, 0, 0 };
                    nodeRotation.VectorD2 = luffAx;

                    model.NodeRotationList.Add(nodeRotation);
                }
                else if (cbs.Find(x => x.Name == "TwinRocker_CB").CheckState != CheckState.Checked &&
                    cbs.Find(x => x.Name == "Luffing_CB").CheckState == CheckState.Checked)
                {
                    nodeRotation = new NodeRotation();
                    nodeRotation.RotationID = 2;
                    nodeRotation.RotationText = "Luff";
                    nodeRotation.IsParallelogram = false;
                    nodeRotation.NodeList = tbs.Find(x => x.Name == "LuffNodesListFront_TB").Text;
                    nodeRotation.RotationAngle = row.Field<double>("Luff");
                    nodeRotation.NodeA1 = UInt32.TryParse(tbs.Find(x => x.Name == "LuffNodeA_TB")?.Text, out result) ? result : 0;
                    nodeRotation.NodeA2 = nodeRotation.NodeA1;
                    nodeRotation.VectorA1 = new double[] { 0, 0, 0 };
                    nodeRotation.VectorA2 = luffAx;
                    model.NodeRotationList.Add(nodeRotation);
                }

                var analysisOption = new AnalysisOption()
                {
                    LoadCaseList = row.Field<string>("Load cases")
                };
                model.AnalysisOptionList.Add(analysisOption);

                var designOption = new DesignOption()
                {
                    DesignGroupsList = row.Field<string>("Design members")
                };
                model.DesignOptionList.Add(designOption);

                if (cbs.Find(x => x.Name == "ExportMdb_CB").CheckState != CheckState.Checked &&
                    cbs.Find(x => x.Name == "ExportTxt_CB").CheckState != CheckState.Checked)
                {
                    //var exportOption = new ExportOption();
                    model.ExportOptionList.Add(new ExportOption());
                }

                if (cbs.Find(x => x.Name == "ExportMdb_CB").CheckState == CheckState.Checked)
                {
                    var exportOption = new ExportOption();
                    exportOption.ExportSectionText = row.Field<string>("Export sections");
                    exportOption.ExportOptionTitle = "Ecxport to MDB";
                    exportOption.ExportMethod = ExportMethod.ExportToMDBFile;
                    model.ExportOptionList.Add(exportOption);
                };

                if (cbs.Find(x => x.Name == "ExportTxt_CB").CheckState == CheckState.Checked)
                {
                    var exportOption = new ExportOption();
                    exportOption.ExportSectionText = row.Field<string>("Export sections");
                    exportOption.ExportOptionTitle = "Ecxport to Text";
                    exportOption.ExportMethod = ExportMethod.ExportToTextFile;
                    model.ExportOptionList.Add(exportOption);
                };

                model.Notes = row.Field<string>("Notes");
                var modelOption = new ModelOption();
                modelOption.NamingStartString = tbs.Find(x => x.Name == "NameStart_TB").Text;
                modelOption.IsCRFModel = cbs.Find(x => x.Name == "CRF_CB").Checked;
                modelOption.MaxPrimaryCase = UInt32.TryParse(tbs.Find(x => x.Name == "PrimaryCasesLimit_TB")?.Text, out result) ? result : 0;
                modelOption.ShallOverWrite = cbs.Find(x => x.Name == "OverwriteOldFiles_CB").Checked;
                modelOption.DeleteTempFiles = cbs.Find(x => x.Name == "CleanTempFiles_CB").Checked;
                modelOption.RunModel = row.Field<bool?>("Run") ?? false;
                modelOption.CreateSGFile = cbs.Find(x => x.Name == "CreateSGFile_CB").Checked;
                modelOption.RunAnalysis = cbs.Find(x => x.Name == "RunAnalysis_CB").Checked;
                modelOption.RunDesign = cbs.Find(x => x.Name == "RunDesign_CB").Checked;
                modelOption.RunExport = cbs.Find(x => x.Name == "ExportTxt_CB").Checked || cbs.Find(x => x.Name == "ExportMdb_CB").Checked;

                int i = 1;
                // Nodes table
                var nodesTable = ds.Tables["Nodes"];
                if (nodesTable != null)
                {
                    DataView dview = new DataView(nodesTable);
                    dview.RowFilter = $"ModelID={id}";

                    foreach (DataRowView drv in dview)
                    {
                        var node = new Nodes()
                        {
                            Node = (uint)drv["Node"],
                            X = (double)drv["X"],
                            Y = (double)drv["Y"],
                            Z = (double)drv["Z"]
                        };

                        var objectChange = new ObjectChange();
                        objectChange.Edit = node;
                        objectChange.ChangeID = i;
                        objectChange.ChangeText = "Nodes";
                        model.ObjectChangeList.Add(objectChange);
                        i++;
                    }
                }

                // Node restraints table
                var nodeRestraintsTable = ds.Tables["NodeRestraints"];
                if (nodeRestraintsTable != null)
                {
                    DataView dview = new DataView(nodeRestraintsTable);
                    dview.RowFilter = $"ModelID ={id}";

                    foreach (DataRowView drv in dview)
                    {
                        var nodeRestraint = new NodeRestraints
                        {
                            Node = (uint)drv["Node"],
                            RestraintCode = (string)drv["Node Restraints"]
                        };
                        var objectChange = new ObjectChange();
                        objectChange.Edit = nodeRestraint;
                        objectChange.ChangeID = i;
                        objectChange.ChangeText = "Node Restraints";
                        model.ObjectChangeList.Add(objectChange);
                        i++;
                    }
                }

                // Node relations table
                var nodeRelationsTable = ds.Tables["NodeRelations"];
                if (nodeRelationsTable != null)
                {
                    DataView dview = new DataView(nodeRelationsTable);
                    dview.RowFilter = $"ModelID={id}";

                    foreach (DataRowView drv in dview)
                    {
                        var nodeRelation = new NodeRelation
                        {
                            RelationID = i,
                            RelationName = "Node Relation",
                            NodeNumber = (uint)drv["Node"],
                            NodeA = (uint)drv["NodeA"],
                            NodeB = (uint)drv["NodeB"],
                            RatioFromA = (double)drv["Ratio"]
                        };
                        model.NodeRelationList.Add(nodeRelation);
                    }
                }

                // Members table
                var membersTable = ds.Tables["Members"];
                if (membersTable != null)
                {
                    DataView dview = new DataView(membersTable);
                    dview.RowFilter = $"ModelID={id}";

                    foreach (DataRowView drv in dview)
                    {
                        var member = new Members
                        {
                            Member = (uint)drv["Member"],
                            NodeA = (uint)drv["NodeA"],
                            NodeB = (uint)drv["NodeB"],
                            FixityCodeatA = (string)drv["FixityA"],
                            FixityCodeatB = (string)drv["FixityB"]
                        };
                        var objectChange = new ObjectChange();
                        objectChange.Edit = member;
                        objectChange.ChangeID = i;
                        objectChange.ChangeText = "Members";
                        model.ObjectChangeList.Add(objectChange);
                        i++;
                    }
                }
            }
            return model;
        }

        private static void UpdateModelFromDS(DataSet ds, Model model, int id,
                                                    List<System.Windows.Forms.CheckBox> cbs,
                                                    List<System.Windows.Forms.GroupBox> gbs,
                                                    List<System.Windows.Forms.TextBox> tbs)
        {
            var modelsTable = ds.Tables["Models"];

            if (modelsTable != null)
            {
                var row = modelsTable.Rows[id - 1];
                model.ModelName = row.Field<string>("Model name");
                model.ModelDescription = row.Field<string>("Model description");
                var moveAx = GetGBValue(gbs.Find(x => x.Name == "MovingAxies_GB"));
                var slewAx = GetGBValue(gbs.Find(x => x.Name == "SlewingAxies_GB"));
                var luffAx = GetGBValue(gbs.Find(x => x.Name == "LuffingAxies_GB"));
                uint result = 0;

                var nodeTranslation = model.NodeTranslationList.Find(x => x.TranslationText == "Moving") ?? new NodeTranslation();
                if (cbs.Find(x => x.Name == "Moving_CB").CheckState == CheckState.Checked)
                {
                    // if NodeTranslationList contains a NodeTranslation with ID=1 and the user ticked moving ON then update it,
                    // if NodeTranslationList does not contains a NodeTranslation with ID=1 and the user ticked moving ON then create new one.
                    nodeTranslation.TranslationID = 1;
                    nodeTranslation.TranslationText = "Moving";
                    nodeTranslation.NodeList = tbs.Find(x => x.Name == "MovingNodesList_TB").Text;
                    nodeTranslation.Distance = row.Field<double>("Translation");
                    nodeTranslation.FixedVectorA = new double[] { 0, 0, 0 };
                    nodeTranslation.FixedVectorB = moveAx;
                    if (model.NodeTranslationList.Find(x => x.TranslationText == "Moving") == null) model.NodeTranslationList.Add(nodeTranslation);
                }
                // if NodeTranslationListcontains a NodeTranslation with ID=1 and the user ticked moving OFF then remove it from the list.
                if (cbs.Find(x => x.Name == "Moving_CB").CheckState != CheckState.Checked &&
                    model.NodeTranslationList.Find(x => x.TranslationText == "Moving") != null) model.NodeTranslationList.RemoveAll(x => x.TranslationID == 1);

                var nodeRotation = model.NodeRotationList.Find(x => x.RotationText == "Slew") ?? new NodeRotation();
                if (cbs.Find(x => x.Name == "Slewing_CB").CheckState == CheckState.Checked)
                {
                    nodeRotation.RotationID = 1;
                    nodeRotation.RotationText = "Slew";
                    nodeRotation.IsParallelogram = false;
                    nodeRotation.NodeList = tbs.Find(x => x.Name == "SlewNodesList_TB").Text;
                    nodeRotation.RotationAngle = row.Field<double>("Slew");
                    nodeRotation.NodeA1 = UInt32.TryParse(tbs.Find(x => x.Name == "SlewNode_TB")?.Text, out result) ? result : 0;
                    nodeRotation.NodeA2 = nodeRotation.NodeA1;
                    nodeRotation.VectorA1 = new double[] { 0, 0, 0 };
                    nodeRotation.VectorA2 = slewAx;
                    if (model.NodeRotationList.Find(x => x.RotationText == "Slew") == null) model.NodeRotationList.Add(nodeRotation);
                };
                if (cbs.Find(x => x.Name == "Slewing_CB").CheckState != CheckState.Checked &&
                model.NodeRotationList.Find(x => x.RotationText == "Slew") != null) model.NodeRotationList.RemoveAll(x => x.RotationText == "Slew");

                if (cbs.Find(x => x.Name == "TwinRocker_CB").CheckState == CheckState.Checked &&
                    cbs.Find(x => x.Name == "Luffing_CB").CheckState == CheckState.Checked)
                {
                    nodeRotation = model.NodeRotationList.Find(x => x.RotationText == "Luff" && x.IsParallelogram == false) ?? new NodeRotation();
                    nodeRotation.RotationID = 2;
                    nodeRotation.RotationText = "Luff";
                    nodeRotation.IsParallelogram = false;
                    nodeRotation.NodeList = tbs.Find(x => x.Name == "LuffNodesListFront_TB").Text;
                    nodeRotation.RotationAngle = row.Field<double>("Luff");
                    nodeRotation.NodeA1 = UInt32.TryParse(tbs.Find(x => x.Name == "LuffNodeA_TB")?.Text, out result) ? result : 0;
                    nodeRotation.NodeA2 = nodeRotation.NodeA1;
                    nodeRotation.VectorA1 = new double[] { 0, 0, 0 };
                    nodeRotation.VectorA2 = luffAx;
                    if (model.NodeRotationList.Find(x => x.RotationText == "Luff" && x.IsParallelogram == false) == null) model.NodeRotationList.Add(nodeRotation);

                    nodeRotation = model.NodeRotationList.Find(x => x.RotationText == "Luff" && x.IsParallelogram == true) ?? new NodeRotation();
                    nodeRotation.RotationID = 3;
                    nodeRotation.RotationText = "Luff";
                    nodeRotation.IsParallelogram = true;
                    nodeRotation.NodeList = tbs.Find(x => x.Name == "LuffNodesListRear_TB").Text;
                    nodeRotation.RotationAngle = row.Field<double>("Luff");
                    nodeRotation.NodeA1 = UInt32.TryParse(tbs.Find(x => x.Name == "LuffNodeA_TB")?.Text, out result) ? result : 0;
                    nodeRotation.NodeA2 = nodeRotation.NodeA1;
                    nodeRotation.VectorA1 = new double[] { 0, 0, 0 };
                    nodeRotation.VectorA2 = luffAx;
                    nodeRotation.NodeB1 = UInt32.TryParse(tbs.Find(x => x.Name == "LuffNodeB_TB")?.Text, out result) ? result : 0;
                    nodeRotation.NodeB2 = nodeRotation.NodeB1;
                    nodeRotation.VectorB1 = new double[] { 0, 0, 0 };
                    nodeRotation.VectorB2 = luffAx;
                    nodeRotation.NodeC1 = UInt32.TryParse(tbs.Find(x => x.Name == "LuffNodeC_TB")?.Text, out result) ? result : 0;
                    nodeRotation.NodeC2 = nodeRotation.NodeC1;
                    nodeRotation.VectorC1 = new double[] { 0, 0, 0 };
                    nodeRotation.VectorC2 = luffAx;
                    nodeRotation.NodeD1 = UInt32.TryParse(tbs.Find(x => x.Name == "LuffNodeD_TB")?.Text, out result) ? result : 0;
                    nodeRotation.NodeD2 = nodeRotation.NodeD1;
                    nodeRotation.VectorD1 = new double[] { 0, 0, 0 };
                    nodeRotation.VectorD2 = luffAx;
                    if (model.NodeRotationList.Find(x => x.RotationText == "Luff" && x.IsParallelogram == true) == null) model.NodeRotationList.Add(nodeRotation);
                }
                else if (cbs.Find(x => x.Name == "TwinRocker_CB").CheckState != CheckState.Checked &&
                    cbs.Find(x => x.Name == "Luffing_CB").CheckState == CheckState.Checked)
                {
                    nodeRotation = model.NodeRotationList.Find(x => x.RotationText == "Luff" && x.IsParallelogram == false) ?? new NodeRotation();
                    nodeRotation.RotationID = 2;
                    nodeRotation.RotationText = "Luff";
                    nodeRotation.IsParallelogram = false;
                    nodeRotation.NodeList = tbs.Find(x => x.Name == "LuffNodesListFront_TB")?.Text;
                    nodeRotation.RotationAngle = row.Field<double>("Luff");
                    nodeRotation.NodeA1 = UInt32.TryParse(tbs.Find(x => x.Name == "LuffNodeA_TB")?.Text, out result) ? result : 0;
                    nodeRotation.NodeA2 = nodeRotation.NodeA1;
                    nodeRotation.VectorA1 = new double[] { 0, 0, 0 };
                    nodeRotation.VectorA2 = luffAx;
                    if (model.NodeRotationList.Find(x => x.RotationText == "Luff" && x.IsParallelogram == false) == null) model.NodeRotationList.Add(nodeRotation);
                }

                var analysisOption = model.AnalysisOptionList[0] ?? new AnalysisOption();
                analysisOption.LoadCaseList = row.Field<string>("Load cases");
                if (model.AnalysisOptionList[0] == null) model.AnalysisOptionList.Add(analysisOption);

                var designOption = model.DesignOptionList[0] ?? new DesignOption();
                designOption.DesignGroupsList = row.Field<string>("Design members");
                if (model.DesignOptionList[0] == null) model.DesignOptionList.Add(designOption);

                if (cbs.Find(x => x.Name == "ExportMdb_CB").CheckState != CheckState.Checked &&
                    cbs.Find(x => x.Name == "ExportTxt_CB").CheckState != CheckState.Checked)
                {
                    model.ExportOptionList.RemoveAll(x => x.ExportOptionTitle == "Export to MDB");
                    model.ExportOptionList.RemoveAll(x => x.ExportOptionTitle == "Ecxport to Text");
                    model.ExportOptionList.Add(new ExportOption());
                }

                if (cbs.Find(x => x.Name == "ExportMdb_CB").CheckState == CheckState.Checked)
                {
                    var exportOption = model.ExportOptionList.Find(x => x.ExportOptionTitle == "Ecxport to MDB") ?? new ExportOption();
                    exportOption.ExportSectionText = row.Field<string>("Export sections");
                    exportOption.ExportOptionTitle = "Ecxport to MDB";
                    exportOption.ExportMethod = ExportMethod.ExportToMDBFile;
                    if (model.ExportOptionList.Find(x => x.ExportOptionTitle == "Ecxport to MDB") == null) model.ExportOptionList.Add(exportOption);
                };
                if (cbs.Find(x => x.Name == "ExportMdb_CB").CheckState != CheckState.Checked &&
                    model.ExportOptionList.Find(x => x.ExportOptionTitle == "Ecxport to MDB") != null)
                    model.ExportOptionList.RemoveAll(x => x.ExportOptionTitle == "Export to MDB");

                if (cbs.Find(x => x.Name == "ExportTxt_CB").CheckState == CheckState.Checked)
                {
                    var exportOption = model.ExportOptionList.Find(x => x.ExportOptionTitle == "Ecxport to Text") ?? new ExportOption();
                    exportOption.ExportSectionText = row.Field<string>("Export sections");
                    exportOption.ExportOptionTitle = "Ecxport to Text";
                    exportOption.ExportMethod = ExportMethod.ExportToTextFile;
                    if (model.ExportOptionList.Find(x => x.ExportOptionTitle == "Ecxport to Text") == null) model.ExportOptionList.Add(exportOption);
                };
                if (cbs.Find(x => x.Name == "ExportMdb_CB").CheckState != CheckState.Checked &&
                    model.ExportOptionList.Find(x => x.ExportOptionTitle == "Ecxport to Text") != null)
                    model.ExportOptionList.RemoveAll(x => x.ExportOptionTitle == "Ecxport to Text");
                int i = 1;
                // Nodes table
                var nodesTable = ds.Tables["Nodes"];
                if (nodesTable != null)
                {
                    DataView dview = new DataView(nodesTable);
                    dview.RowFilter = $"ModelID={id}";

                    foreach (DataRowView drv in dview)
                    {
                        var node = new Nodes()
                        {
                            Node = (uint)drv["Node"],
                            X = (double)drv["X"],
                            Y = (double)drv["Y"],
                            Z = (double)drv["Z"]
                        };

                        var objectChange = model.ObjectChangeList.Find(x => x.ChangeText == "Nodes") ?? new ObjectChange();
                        objectChange.Edit = node;
                        objectChange.ChangeID = i;
                        objectChange.ChangeText = "Nodes";
                        if (model.ObjectChangeList.Find(x => x.ChangeText == "Nodes") == null) model.ObjectChangeList.Add(objectChange);
                        i++;
                    }
                    if (dview.Count == 0 &&
                        model.ObjectChangeList.Find(x => x.ChangeText == "Nodes") != null)
                        model.ObjectChangeList.RemoveAll(x => x.ChangeText == "Nodes");
                }
                else
                {
                    if (model.ObjectChangeList.Find(x => x.ChangeText == "Nodes") != null)
                        model.ObjectChangeList.RemoveAll(x => x.ChangeText == "Nodes");
                }

                // Node restraints table
                var nodeRestraintsTable = ds.Tables["NodeRestraints"];
                if (nodeRestraintsTable != null)
                {
                    DataView dview = new DataView(nodeRestraintsTable);
                    dview.RowFilter = $"ModelID ={id}";

                    foreach (DataRowView drv in dview)
                    {
                        var nodeRestraint = new NodeRestraints
                        {
                            Node = (uint)drv["Node"],
                            RestraintCode = (string)drv["Node restraints"]
                        };
                        var objectChange = model.ObjectChangeList.Find(x => x.ChangeText == "Node Restraints") ?? new ObjectChange();
                        objectChange.Edit = nodeRestraint;
                        objectChange.ChangeID = i;
                        objectChange.ChangeText = "Node Restraints";
                        if (model.ObjectChangeList.Find(x => x.ChangeText == "Node Restraints") == null) model.ObjectChangeList.Add(objectChange);
                        i++;
                    }
                    if (dview.Count == 0 &&
                        model.ObjectChangeList.Find(x => x.ChangeText == "Node Restraints") != null)
                        model.ObjectChangeList.RemoveAll(x => x.ChangeText == "Node Restraints");
                }
                else
                {
                    if (model.ObjectChangeList.Find(x => x.ChangeText == "Node Restraints") != null)
                        model.ObjectChangeList.RemoveAll(x => x.ChangeText == "Node Restraints");
                }

                // Node relations table
                var nodeRelationsTable = ds.Tables["NodeRelations"];
                if (nodeRelationsTable != null)
                {
                    DataView dview = new DataView(nodeRelationsTable);
                    dview.RowFilter = $"ModelID={id}";
                    foreach (DataRowView drv in dview)
                    {
                        var nodeRelation = model.NodeRelationList.Find(x => x.RelationName == "Node Relation") ?? new NodeRelation();
                        nodeRelation.RelationID = i;
                        nodeRelation.RelationName = "Node Relation";
                        nodeRelation.NodeNumber = (uint)((drv["Node"] == DBNull.Value) ? uint.MinValue : drv["Node"]);
                        nodeRelation.NodeA = (uint)drv["NodeA"];
                        nodeRelation.NodeB = (uint)drv["NodeB"];
                        nodeRelation.RatioFromA = (double)drv["Ratio"];
                        if (model.NodeRelationList.Find(x => x.RelationName == "Node Relation") == null) model.NodeRelationList.Add(nodeRelation);
                    }
                    if (dview.Count == 0 &&
                        model.NodeRelationList.Find(x => x.RelationName == "Node Relation") != null)
                        model.NodeRelationList.RemoveAll(x => x.RelationName == "Node Relation");
                }
                else
                {
                    if (model.NodeRelationList.Find(x => x.RelationName == "Node Relation") != null) model.NodeRelationList.RemoveAll(x => x.RelationName == "Node Relation");
                }

                // Members table
                var membersTable = ds.Tables["Members"];
                if (membersTable != null)
                {
                    DataView dview = new DataView(membersTable);
                    dview.RowFilter = $"ModelID={id}";

                    foreach (DataRowView drv in dview)
                    {
                        var member = new Members
                        {
                            Member = (uint)drv["Member"],
                            NodeA = (uint)drv["NodeA"],
                            NodeB = (uint)drv["NodeB"],
                            FixityCodeatA = (string)drv["FixityA"],
                            FixityCodeatB = (string)drv["FixityB"]
                        };
                        var objectChange = model.ObjectChangeList.Find(x => x.ChangeText == "Members") ?? new ObjectChange(); ;
                        objectChange.Edit = member;
                        objectChange.ChangeID = i;
                        objectChange.ChangeText = "Members";
                        if (model.ObjectChangeList.Find(x => x.ChangeText == "Members") == null) model.ObjectChangeList.Add(objectChange);
                        i++;
                    }
                    if (dview.Count == 0 &&
                        model.ObjectChangeList.Find(x => x.ChangeText == "Members") != null)
                        model.ObjectChangeList.RemoveAll(x => x.ChangeText == "Members");
                }
                else
                {
                    if (model.ObjectChangeList.Find(x => x.ChangeText == "Members") != null)
                        model.ObjectChangeList.RemoveAll(x => x.ChangeText == "Members");
                }

                // Other model properties
                model.Notes = row.Field<string>("Notes");
                model.ModelDescription = row.Field<string>("Model description");
                model.ModelName = row.Field<string>("Model name");
                var modelOption = model.ModelOption;
                modelOption.NamingStartString = tbs.Find(x => x.Name == "NameStart_TB").Text;
                modelOption.IsCRFModel = cbs.Find(x => x.Name == "CRF_CB").Checked;
                modelOption.MaxPrimaryCase = UInt32.TryParse(tbs.Find(x => x.Name == "PrimaryCasesLimit_TB")?.Text, out result) ? result : 0;
                modelOption.ShallOverWrite = cbs.Find(x => x.Name == "OverwriteOldFiles_CB").Checked;
                modelOption.DeleteTempFiles = cbs.Find(x => x.Name == "CleanTempFiles_CB").Checked;
                modelOption.RunModel = row.Field<bool>("Run");
                modelOption.CreateSGFile = cbs.Find(x => x.Name == "CreateSGFile_CB").Checked;
                modelOption.RunAnalysis = cbs.Find(x => x.Name == "RunAnalysis_CB").Checked;
                modelOption.RunDesign = cbs.Find(x => x.Name == "RunDesign_CB").Checked;
                modelOption.RunExport = cbs.Find(x => x.Name == "ExportTxt_CB").Checked || cbs.Find(x => x.Name == "ExportMdb_CB").Checked;
            }
        }

private static double[] GetGBValue(GroupBox groupBox)
        {
            double[] result = new double[3];

            foreach (Control control in groupBox?.Controls)
            {
                if (control is RadioButton)
                {
                    RadioButton radioButton = (RadioButton)control;
                    if (radioButton.Checked)
                    {
                        if (radioButton.Name.Contains("1"))
                        {
                            result[0] = 1;
                        }
                        else if (radioButton.Name.Contains("2"))
                        {
                            result[1] = 1;
                        }
                        else if (radioButton.Name.Contains("3"))
                        {
                            result[2] = 1;
                        }
                    }
                }
            }
            return result;
        }
