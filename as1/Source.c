/* COP 3502C Assignment 1
This program is written by: Judah Libera */
//VS 2019 seems to have an issue with access violation, but runs fine everywhere else i've tested
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> 

typedef struct monster {
    char* name;
    char* element;
    int population;
} monster;

typedef struct region {
    char* name;
    int nmonsters;
    int total_population;
    monster** monsters;
} region;

typedef struct itinerary {
    int nregions;
    region** regions;
    int captures;
} itinerary;

typedef struct trainer {
    char* name;
    itinerary* visits;
} trainer;

monster* createMonster(char* name, char* element, int population)//dynamically allocates monster filled with name element and population
{

    monster* monster;
    monster = malloc(sizeof(monster));
    int nlen = strlen(name) + 1;
    int elen = strlen(element) + 1;
    monster->name = (char*)malloc(nlen * sizeof(char)); //creates room to hold name
    strcpy(monster->name, name);
    monster->element = (char*)malloc(elen * sizeof(char)); //creates room to hold name
    strcpy(monster->element, element);
    monster->population = population;
    return monster;
}

monster** readMonsters(FILE* infile, int* monsterCount)
{
    char garbagedata[51];
    fscanf(infile, "%d %s", monsterCount, garbagedata);
    monster** ma; //ma needs to points to the dynamically allocated monsters
    ma = (monster**)malloc(*monsterCount * sizeof(monster*));
    for (int i = 0; i < *monsterCount; i++) // reads in and and calls function to use monster properties for each monster in the file
    {
        char name[51];
        char element[51];
        int population;
        fscanf(infile, "%s %s %d", name, element, &population);
        ma[i] = createMonster(name, element, population);
    }
    return ma; //returns pointer with all of the monsters in it
}

region** readRegions(FILE* infile, int* countRegions, monster** monsterList, int monsterCount)
{
    char garbagedata[51];
    fscanf(infile, "%d %s", countRegions, garbagedata);
    region** rgn;
    rgn = (region**)malloc(*countRegions * sizeof(region*));
    for (int i = 0; i < *countRegions; i++) // loops for each region in the file
    {
        rgn[i] = (region*)malloc(sizeof(region*));
        char name[51];
        fscanf(infile, "%s", name);
        int nlen = strlen(name) + 1;
        rgn[i]->name = (char*)malloc(nlen * sizeof(char)); //creates room to hold name
        strcpy(rgn[i]->name, name);
        fscanf(infile, "%d %s", &rgn[i]->nmonsters, garbagedata);
        rgn[i]->monsters = (monster**)malloc(rgn[i]->nmonsters * sizeof(monster*));
        char mname[51];
        for (int k = 0; k < rgn[i]->nmonsters; k++) //loops per monster in the region
        {
            fscanf(infile, "%s", mname);
            for (int j = 0; j < monsterCount; j++) //goes through every monster
            {
                if (strcmp(mname, monsterList[j]->name) == 0) //if the current monster is one of the ones in region, add it to the monsters part of the region pointer. also add it to the regions total_population
                {
                    rgn[i]->monsters[k] = monsterList[j];
                    rgn[i]->total_population += monsterList[j]->population;
                }
            }
        }
    }
    return rgn;
}

trainer* readTrainers(FILE* infile, int* trainerCount, region** regionList, int countRegions)
{
    char garbagedata[51];
    fscanf(infile, "%d %s", trainerCount, garbagedata);
    trainer* trnr;
    trnr = (trainer*)malloc(*trainerCount * sizeof(trainer));
    for (int i = 0; i < *trainerCount; i++) // loops for each trainer in the file
    {
        char name[51];
        fscanf(infile, "%s", name);
        int nlen = strlen(name) + 1;
        trnr[i].name = (char*)malloc(nlen * sizeof(char)); //creates room to hold name
        strcpy(trnr[i].name, name);
        trnr[i].visits = (itinerary*)malloc(sizeof(itinerary)); // allocates and stores data within itinerary
        fscanf(infile, "%d %s", &trnr[i].visits->captures, garbagedata);
        fscanf(infile, "%d %s", &trnr[i].visits->nregions, garbagedata);
        trnr[i].visits->regions = (region**)malloc(trnr[i].visits->nregions * sizeof(region*));
        char mname[51];
        for (int k = 0; k < trnr[i].visits->nregions; k++) //loops for each region in itinerary
        {
            fscanf(infile, "%s", mname);
            for (int j = 0; j < countRegions; j++)
            {
                if (strcmp(mname, regionList[j]->name) == 0)//if the current region is one of the ones the trainer goes to, add it to the regions part of the itinerary pointer in trainer.
                {
                    trnr[i].visits->regions[k] = regionList[j];
                }
            }
        }
    }
    return trnr;
}

void process_inputs(monster** monsterList, int monsterCount, region** regionList, int regionCount, trainer* trainerList, int trainerCount)
{
    FILE* ofp = fopen("out.txt", "w");
    for (int i = 0; i < trainerCount; i++)
    {
        float estimatedcaptureamountfinal;
        float estimatedcaptureamount1;
        float estimatedcaptureamount2;
        printf("%s\n", trainerList[i].name);
        fprintf(ofp, "%s\n", trainerList[i].name);
        for (int j = 0; j < trainerList[i].visits->nregions; j++) //loops for each region the trainer visits
        {
            printf("%s\n", trainerList[i].visits->regions[j]->name);
            fprintf(ofp, "%s\n", trainerList[i].visits->regions[j]->name);
            for (int k = 0; k < trainerList[i].visits->regions[j]->nmonsters; k++) //loops per monster in that region
            {
                if (trainerList[i].visits->regions[j]->total_population == 0) //cathes a divide by 0 error
                {
                    estimatedcaptureamountfinal = 0;
                }
                else //broken up to simplify
                {
                    estimatedcaptureamount1 = ((float)(trainerList[i].visits->regions[j]->monsters[k]->population) / (float)(trainerList[i].visits->regions[j]->total_population));
                    estimatedcaptureamount2 = (float)(trainerList[i].visits->captures);
                    estimatedcaptureamountfinal = round(estimatedcaptureamount1 * estimatedcaptureamount2);
                }
                if (estimatedcaptureamountfinal != 0) // example output file doesn't display a monster if none of them will be caught
                {
                    printf("%.0f %s\n", estimatedcaptureamountfinal, trainerList[i].visits->regions[j]->monsters[k]->name);
                    fprintf(ofp, "%.0f %s\n", estimatedcaptureamountfinal, trainerList[i].visits->regions[j]->monsters[k]->name);
                }
            }
        }
        if (i < (trainerCount - 1)) // sample output is missing the new line on the last trainer output
        {
            printf("\n");
            fprintf(ofp, "\n");
        }
    }
    fclose(ofp);
}

void release_memory(monster** monsterList, int monsterCount, region** regionList, int regionCount, trainer* trainerList, int trainerCount) //follows readfunctions and does each one in reverse order
{
    for (int i = 0; i < trainerCount; i++)
    {
        free(trainerList[i].name);
        free(trainerList[i].visits->regions);
        free(trainerList[i].visits);
    }
    free(trainerList);

    for (int i = 0; i < regionCount; i++)
    {
        free(regionList[i]->name);
        free(regionList[i]->monsters);
        free(regionList[i]);
    }
    free(regionList);

    for (int i = 0; i < monsterCount; i++)
    {
        free(monsterList[i]->name);
        free(monsterList[i]->element);
        free(monsterList[i]);
    }
    free(monsterList);
}

int main() //runs through functions in given order
{
    int mc = 0;
    int cr = 0;
    int tc = 0;
    FILE* ifp = fopen("in.txt", "r");
    monster** m = readMonsters(ifp, &mc);
    region** r = readRegions(ifp, &cr, m, mc);
    trainer* t = readTrainers(ifp, &tc, r, cr);
    process_inputs(m, mc, r, cr, t, tc);
    release_memory(m, mc, r, cr, t, tc);
    fclose(ifp);
    return 0;
}