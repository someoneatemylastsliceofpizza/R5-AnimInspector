# R5-AnimInspector
A Source .qc generator from Apex legends animation file(.rseq, .rrig and .rmdl)

---

### Usage

```bash
R5-AnimInspector.exe <parent directory> [-v <.rseq version>]
```

>**Versions**
> S3 (-v 7)\
> S25 (-v 12)\
> S26 (-v 12.1)

>**Folder Structure**\
>├── model.rrig         # Rig file (must be in parent directory)\
>├── model.rmdl         # Model file (must be in parent directory)\
>└── path/
>> └── to/
>>> └── rseq/
>>>>> ├── *.rseq  # Animation sequences (can be anywhere)\
>>>>> ├── ...