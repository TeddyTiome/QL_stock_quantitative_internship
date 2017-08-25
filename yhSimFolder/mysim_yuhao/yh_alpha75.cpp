#include "include/qlSim.h"
namespace
{
class alpha75: public AlphaBase
{
    public:
    explicit alpha75(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")),
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")), 
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_low")),
        Num1(cfg->getAttributeIntDefault("para1",4)),
	Num2(cfg->getAttributeIntDefault("para2",50)),
	Num3(cfg->getAttributeIntDefault("para3",12))

    {
    	cor.resize(GLOBAL::Dates.size());
    	for (int di =0; di<GLOBAL::Dates.size(); ++di)
    	{
    		cor[di].resize(GLOBAL::Instruments.size(),nan);
    	}

		cor2.resize(GLOBAL::Dates.size());
    	for (int di =0; di<GLOBAL::Dates.size(); ++di)
    	{
    		cor2[di].resize(GLOBAL::Instruments.size(),nan);
    	}

    	adv50.resize(GLOBAL::Dates.size());
    	for (int di =0; di<GLOBAL::Dates.size(); ++di)
    	{
    		adv50[di].resize(GLOBAL::Instruments.size(),nan);
    	}

    	lowm.resize(GLOBAL::Dates.size());
    	for (int di =0; di<GLOBAL::Dates.size(); ++di)
    	{
    		lowm[di].resize(GLOBAL::Instruments.size(),nan);
    	}

    }
    void generate(int di) override
    {

	
        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
            	vector<float> c1(Num1,nan);
            	vector<float> c2(Num1,nan);
            	vector<float> c3(Num2,nan);
         		for (int a =0;a<Num1;++a){
         			c1[a]=vwap[di-delay-a][ii];
         			c2[a]=vol[di-delay-a][ii];
         		}
         		cor[di-delay][ii]=QL_Oputils::corr(c1,c2);
         		
         			for (int c=0; c<Num2;++c ){
         				c3[c]=vol[di-delay-c][ii];
         			}
         			adv50[di-delay][ii]=QL_Oputils::mean(c3);

         		lowm[di-delay][ii]=low[di-delay][ii];
         	}
         }


         for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
        	vector<float> c4(Num1,nan);
        	vector<float> c5(Num1,nan);
        	for (int d =0;d<Num1;++d){
                //QL_Oputils::rank(lowm[di-delay-d]);
                //QL_Oputils::rank(adv50[di-delay-d]);
         			c4[d]=lowm[di-delay-d][ii];
         			c5[d]=adv50[di-delay-d][ii];
         		}
         		cor2[di-delay][ii]=QL_Oputils::corr(c4,c5);
        }


		 for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            //QL_Oputils::rank(cor[di-delay]);
            //QL_Oputils::rank(cor2[di-delay]);

        	if (cor[di-delay][ii]<cor2[di-delay][ii])
                {alpha[ii]=1;}
        	else 
                {alpha[ii]=0;}

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
        }
	const QL_MATRIX<QL_FLOAT> &vwap;
	const QL_MATRIX<QL_FLOAT> &vol;
	const QL_MATRIX<QL_FLOAT> &low;
	vector<vector<float> > cor;
	vector<vector<float> > cor2;
	vector<vector<float> > adv50;
	vector<vector<float> > lowm;
        int Num1; 
	int Num2;
	int Num3;

};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new alpha75(cfg);
        return str;
    }
}

