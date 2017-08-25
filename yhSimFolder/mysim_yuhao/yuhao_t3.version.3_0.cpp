#include "include/qlSim.h"
namespace
{
class t3: public AlphaBase
{
    public:
    explicit t3(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        close(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_close")),
        high(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_high")),
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_low")),
        Num1(cfg->getAttributeIntDefault("para1",5)),
	    Num2(cfg->getAttributeIntDefault("para2",20)),
        Num3(cfg->getAttributeIntDefault("para3",10))

    {

          

        ma5.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            ma5[di].resize(GLOBAL::Instruments.size(),nan);
        }        

        ma20.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            ma20[di].resize(GLOBAL::Instruments.size(),nan);
        }      

        ts5.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            ts5[di].resize(GLOBAL::Instruments.size(),nan);
        }        

        ts20.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            ts20[di].resize(GLOBAL::Instruments.size(),nan);
        }  
        std.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            std[di].resize(GLOBAL::Instruments.size(),nan);
        }   


    }
    void generate(int di) override
    {

    

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

                vector<float> c1(Num1,nan);

                for (int j = 0; j < Num1; ++ j)
                {
                    c1[j] = close[di - delay - j][ii];
                }  

                ma5[di-delay][ii] = QL_Oputils::mean(c1);                

                vector<float> c2(Num2,nan);

                for (int i = 0; i < Num2; ++ i)
                {
                    c2[i] = close[di - delay - i][ii];
                }  

                ma20[di-delay][ii] = QL_Oputils::mean(c2);
            }
        }

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

                vector<float> c1(Num1,nan);

                for (int j = 0; j < Num1; ++ j)
                {
                    c1[j] = close[di - delay - j][ii];
                }  
                QL_Oputils::rank(c1);

                ts5[di-delay][ii] = c1[0];                

                vector<float> c2(Num2,nan);

                for (int i = 0; i < Num2; ++ i)
                {
                    c2[i] = close[di - delay - i][ii];
                }  
                QL_Oputils::rank(c2);

                ts20[di-delay][ii] = c2[0];
            }
        }

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

                vector<float> c1(Num3,nan);

                for (int j = 0; j < Num3; ++ j)
                {
                    c1[j] = close[di - delay - j][ii];
                }  

                std[di-delay][ii] = QL_Oputils::std(c1) ;
            }
        }

        QL_Oputils::rank(std[di-delay]);
        QL_Oputils::rank(ma5[di-delay]);
        QL_Oputils::rank(ma20[di-delay]);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                if (!(std[di-delay][ii] == 0)&& !GLOBALFUNC::isnan(std[di-delay][ii])&&!(ts5[di-delay][ii] == 0)&& !GLOBALFUNC::isnan(std[di-delay-1][ii])&&!(std[di-delay][ii] == 0))
                {
                if (ts5[di-delay][ii]<ts20[di-delay][ii])
                {
                    alpha[ii] = -1* (ma20[di-delay][ii]-ma5[di-delay][ii])/std[di-delay][ii]-(ma20[di-delay-1][ii]-ma5[di-delay-1][ii])/ std[di-delay-1][ii];
                }
                else 
                {
                    alpha[ii] = -1*(ma5[di-delay][ii]-ma20[di-delay][ii])/std[di-delay][ii]-(ma5[di-delay-1][ii]-ma20[di-delay-1][ii])/ std[di-delay-1][ii];
                      
                }
                }
             else {alpha[ii] = 0;}

	       }
        }


        return;


		}
		
    
    
    
    void checkPointSave(boost::archive::binary_oarchive &ar) 
    {
        ar & *this;
    }
    void checkPointLoad(boost::archive::binary_iarchive &ar)
    {
        ar & *this;
    }
    std::string version() const
    {
        return GLOBALCONST::VERSION;
    }
    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int/*file_version*/)
        {
           
            ar & ma5;
            ar & ma20;
            ar & ts5;
            ar & ts20;
            ar & std;
        }
    const QL_MATRIX<QL_FLOAT> &close;
    const QL_MATRIX<QL_FLOAT> &high;
    const QL_MATRIX<QL_FLOAT> &low;

    vector<vector<float> > ma5;
    vector<vector<float> > ma20;
    vector<vector<float> > ts5;
    vector<vector<float> > ts20;
    vector<vector<float> > std;

    int Num1; 
	int Num2;
    int Num3;
    int Num4;
};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new t3(cfg);
        return str;
    }
}

