include
  (Rely.Make)(struct
                let config =
                  Rely.TestFrameworkConfig.initialize
                    {
                      snapshotDir = "./test/lib/__snapshots__";
                      projectDir = "."
                    }
              end)